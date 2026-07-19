## Context

The holiday calendar makes a round trip on every leg build, losing its structure at the top and rebuilding it at the bottom:

```
LegFactory holds QCBusinessCalendar          std::set<QCDate>
   │
   │  getHolidays() → vector<QCDate>          structure discarded, 1x per leg
   ▼                                          (LegFactory.cpp:49,175,331,480,654,818…)
QCInterestRatePeriodsFactory
   stores shared_ptr<vector<QCDate>>
   │
   │  QCDate::businessDay(vector, rule)
   ▼
   QCBusinessCalendar busCal{ result, 1 };    REBUILDS the calendar
   for (fecha : calendar)                     N set inserts,
       busCal.addHoliday(fecha);              each O(log N) expensive comparisons
   busCal.nextBusinessDay(result);            ONE lookup
   // destroyed                               1x PER ADJUSTMENT
```

`QCDate::operator<` calls `excelSerial()`, which recomputes the full Julian-date expression on every comparison, so each of those inserts is dear. `shift(n)` calls `businessDay` n times, multiplying the rebuild.

`QCBusinessCalendar` already implements every branch the adjustment needs — `nextBusinessDay`, `previousBusinessDay`, `modNextBusinessDay` — and is called directly by `LegFactory` (`LegFactory.cpp:219,1044`). The correct pattern is present in the codebase; the hot path simply routes around it. The only thing `QCDate::businessDay` contributes is the `switch` over `QCBusDayAdjRules`.

### Measurement

Isolating holiday count from schedule work (leg spans 2024–2034 with a fixed 120 in-range holidays; the set is inflated with padding holidays in 1900–2020 that the leg never looks up):

| set size | ms/leg | vs baseline |
|---|---|---|
| 0 | 0.045 | 1x |
| 120 | 9.63 | 214x |
| 420 | 43.6 | 969x |

Calendar span had no effect (40y from 2020 vs 200y from 1900: 0.045 vs 0.039 ms/leg), confirming the cost tracks set size, not date range.

`sample(1)` over a 5-second build loop against a 420-holiday calendar:

```
buildBulletFixedRateLeg                         3598
└─ QCInterestRatePeriodsFactory::getPeriods     3598
   └─ _buildBasicDates2                         1358
      └─ QCDate::monthDiffDayRemainder          1346
         └─ QCDate::businessDay(shared_ptr…)    1340
            └─ QCDate::businessDay(vector&…)    1209
               ├─ QCDate::operator<              992   set-rebuild comparisons
               └─ operator new                    88   set node allocations
```

The 0-holiday row looked like the ceiling once the rebuild is gone (~0.045 ms/leg). It was not — see the by-value decision below; the realised figure at 420 holidays was 0.242 ms/leg.

## Goals / Non-Goals

**Goals:**
- Remove the per-adjustment calendar reconstruction from the leg-building path.
- Put rule-dispatched business-day adjustment on `QCBusinessCalendar`, where the holiday set lives.
- Produce numerically identical results for every existing input.
- Delete the `QCDate` calendar overloads that exist only to rebuild a calendar.

**Non-Goals:**
- Eliminating the residual per-leg calendar copy (see "Storing by value" below). 180x was enough; the remaining factor is a known, documented ceiling.
- Caching `excelSerial()` inside `QCDate`. It would further cheapen `operator<`, but after this change the comparisons happen O(log N) times per adjustment instead of O(N log N), so the remaining win is small. Separate change if it ever matters.
- Breaking the Python API. `QCDate.business_day` stays callable with a list of dates.
- The C++ batch operation builder. This change removes its motivation for portfolio construction; if it is still wanted for other reasons, that is a separate proposal.
- Making weekend days configurable, or touching `Portfolio`, `Operation`, or the batch queries.

## Decisions

### The substitution is provably behavior-preserving

The calendar that `QCDate::businessDay` builds is `QCBusinessCalendar{result, 1}` plus every holiday from the vector. For it to differ from the caller's real calendar, some construction argument would have to matter. None does:

- `_startDate` and `_length` are inert. `insertNewYear()` is commented out of the constructor (`QCBusinessCalendar.cpp:15`), and `_length` is read only by `getLength()`. Confirmed empirically: 40-year and 200-year spans produced identical timings and identical results.
- `_firstDayOfWeekend` / `_secondDayOfWeekEnd` are private, hardcoded to Saturday/Sunday in the constructor, and have no setter anywhere in the codebase.
- The holiday content is by construction the same set, since the vector came from `getHolidays()` on the caller's calendar.

So the rebuilt calendar and the caller's calendar are functionally indistinguishable. Passing the original instead of rebuilding it cannot change a result — only the time taken. This is what makes the change safe to make broadly in one pass rather than incrementally.

### `QCInterestRatePeriodsFactory` stores the calendar by value

A `const QCBusinessCalendar&` member would make the factory non-assignable and risk dangling if a caller built the factory from a temporary. That risk turned out to be real rather than hypothetical: `QCFactoryFunctions` constructs the factory as `QCInterestRatePeriodsFactory{…, asCalendar(calendar), …}`, passing a temporary. A reference member would dangle immediately at ten call sites. By-value storage is what makes those sites safe.

Storing by value costs one `std::set` copy per factory construction — once per leg. That is no worse than before, where `LegFactory` performed a full copy into a `vector` at exactly the same point.

The trade-off is visible in the final numbers: per-leg time still rises from 0.036 ms (no holidays) to 0.242 ms (420 holidays), and that residual *is* this copy. It caps the win at 180x rather than the ~1000x the 0-holiday row suggested. Removing it means `shared_ptr<const QCBusinessCalendar>` plus a lifetime contract at every construction site — worth doing only if leg building becomes a bottleneck again.

Function *parameters* throughout take `const QCBusinessCalendar&`.

### `monthDiffDayRemainder` stays on `QCDate`

It is genuine date arithmetic — counting whole months and a day remainder between two dates — that happens to need an adjustment rule applied to its endpoints. That is a `QCDate` responsibility. It changes signature to take `const QCBusinessCalendar&` and calls `calendar.businessDay(...)` internally.

`QCDate::shift` is kept for the same reason: it sequences adjustments and carries `QCSettlementLagBehaviour`, which `QCBusinessCalendar::shift` has no notion of. Only `businessDay` (pure dispatch, moved) and `addWeeks` (no live caller) are removed.

### `QCDate.business_day` is preserved as a binder shim

`source/qcf_binder.cpp:148` binds the `vector&` overload as `QCDate.business_day`, so it is public Python API. Rather than couple an internal cleanup to an API break, the binder keeps the name via a lambda:

```cpp
.def("business_day", [](const QCDate& d, std::vector<QCDate>& holidays,
                        QCDate::QCBusDayAdjRules rule) {
        QCBusinessCalendar cal{d, 1};
        for (const auto& h : holidays) cal.addHoliday(h);
        return cal.businessDay(d, rule);
})
```

This is exactly as slow as today's implementation, which is acceptable: it is now only reachable from direct Python calls, never from leg building. It also keeps the pathological pattern in exactly one visible place, which is a fair marker if the method should later be deprecated in favour of `BusinessCalendar.business_day`.

`BusinessCalendar` gains a `business_day` binding as the preferred surface.

### `QCFactoryFunctions` is adapted, not deleted

The proposal called for deleting `QCFactoryFunctions.cpp` and `QCDiscountBondPayoff.cpp`, on the evidence that the active binder references neither. That evidence was incomplete — it established only that the *binder* does not reach them, not that nothing does.

`QCFXForward.cpp` and `QCTimeDepositPayoff.cpp` are both in the live `target_sources` block. `QCFXForward.h` includes `QCDiscountBondPayoff.h` and declares `vector<shared_ptr<QCDiscountBondPayoff>> _legs`; `QCDiscountBondPayoff.cpp` in turn includes `QCFactoryFunctions.h`. Deleting the two files therefore requires deleting `QCFXForward`, `QCTimeDepositPayoff`, and the curve-bootstrapping classes that include them — a large deletion that may well be correct but is a different change with a different risk profile.

So both files stay. `QCFactoryFunctions.cpp` keeps its `vector<QCDate>` public signatures, which its callers depend on, and adapts at the ten periods-factory construction sites and thirteen adjustment sites through a file-scope helper:

```cpp
namespace {
    QCBusinessCalendar asCalendar(const std::vector<QCDate>& holidays);
}
```

This rebuilds a calendar per call exactly as the old `QCDate::businessDay` did. That is acceptable precisely because nothing on a hot path routes here — and it concentrates the old pattern in one named, greppable place if the subtree is later removed.

Three further files that a grep for `businessDay` reports are already outside every build target and are left untouched:

| File | Sites | Why it does not build |
|---|---|---|
| `source/QC_DVE_PYBIND.cpp` | 1 | in no target's source list |
| `include/QCDvePyBindHelperFunctions.h` | 8 | included only by `QC_DVE_PYBIND.cpp` |
| `source/QC_Financial.cpp` | 2 | listed in `QC_FINANCIAL_SOURCES` (`source/CMakeLists.txt:80`), a variable no target consumes |

`QC_Financial.cpp` matters because it takes the address of both `monthDiffDayRemainder` overloads explicitly, which would break under the signature change — it does not, because it is never compiled.

## Risks / Trade-offs

- **Wide mechanical diff.** ~28 call sites in `QCInterestRatePeriodsFactory.cpp` alone. Mitigated by the behavior-preservation argument above and by the fact that the compiler catches every missed site: the old overloads are gone, so anything not updated fails to build rather than silently misbehaving.
- **`addWeeks` deletion is a judgement call.** Its only caller is the unbuilt `QCDvePyBindHelperFunctions.h`, and it is not bound in `qcf_binder.cpp`. If it is wanted later it belongs on `QCBusinessCalendar` next to `shift`, not on `QCDate`.
- **`QCDate::shift` was nearly deleted in error.** The proposal called it a duplicate of `QCBusinessCalendar::shift`. It is not: it handles `QCSettlementLagBehaviour` (applying an adjustment before the loop when `qcMoveToWorkingDay`), and derives direction from an adjustment rule rather than the sign of `nDays`. `calendar.shift(d, 0)` returns `d` unchanged where `d.shift(cal, 0, qcFollow, qcMoveToWorkingDay)` adjusts it. It is kept, with only its calendar parameter changed.
- **The measurement is single-machine, single-config** (cp312, x86_64, one leg shape). The mechanism is representation-level rather than microarchitectural, so the improvement should hold broadly.

## Verification

Both checks were run.

**Correctness — passed.** `Leg.record()` output was captured before the change across 65 cases and 776 cashflow rows: fixed-rate over every adjustment rule x every stub convention x two settlement lags, custom amortization, Ibor (exercising the fixing calendar and fixing lag), overnight index (separate fixing adjustment rule), and ICP-CLP, plus a direct `business_day` sweep over holiday, weekend, and month-end dates. Cashflow types without `record()` fall back to their start/end/settlement dates, which are the fields date arithmetic would actually shift. Re-running against the rebuilt wheel produced **byte-identical output in all 65 cases**, with no key added or dropped.

**Performance — passed, below prediction.** The 420-holiday case fell from 43.6 to 0.242 ms/leg (180x); 120 holidays from 9.63 to 0.107 (90x). The predicted ~1000x assumed the 0-holiday row as the ceiling, which ignored the per-leg calendar copy introduced by by-value storage. See the by-value decision above.

**Build — passed.** All four supported pyenv versions (3.11.15, 3.12.13, 3.13.13, 3.14.5) build clean via `./compile.sh`.

**Python surface — passed.** `QCDate.business_day(holiday_list, rule)` is unchanged and agrees with the new `BusinessCalendar.business_day(date, rule)` across every rule on holiday, weekend, and month-end inputs.
