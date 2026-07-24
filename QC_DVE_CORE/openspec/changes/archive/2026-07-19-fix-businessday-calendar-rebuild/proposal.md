# Proposal: fix-businessday-calendar-rebuild

## Why

Building a leg is two orders of magnitude slower than it should be, because every business-day adjustment reconstructs the entire holiday calendar to answer a single query.

`QCDate::businessDay(vector<QCDate>&, rule)` (`source/time/QCDate.cpp:329`) creates a fresh `QCBusinessCalendar`, inserts all N holidays into its `std::set`, performs one lookup, and destroys it. `QCInterestRatePeriodsFactory` calls it once per period, and `LegFactory` feeds it a flattened `vector<QCDate>` obtained from a `QCBusinessCalendar` it already holds — so the structure is discarded and rebuilt on every call.

Measured on a 10-year semiannual bullet fixed-rate leg (cp312, x86_64), varying only the holiday count while holding the in-range holidays and all schedule work constant:

| holidays in calendar | ms/leg | vs empty |
|---|---|---|
| 0 | 0.045 | 1x |
| 120 | 9.63 | 214x |
| 420 | 43.6 | 969x |

Calendar *span* is irrelevant (40y vs 200y: 0.045 vs 0.039 ms). Only holiday count matters, at roughly 80 µs per holiday per leg. A realistic Chilean calendar sits near the 420 row, so a 50,000-operation two-leg portfolio costs about 72 minutes to build.

A 5-second sampling profile attributes 1209 of 3598 samples to `QCDate::businessDay(vector&)`, of which ~1000 are `QCDate::operator<` — the comparisons performed while re-inserting holidays into the rebuilt set.

This was found while investigating whether a C++ batch operation builder would fix slow portfolio construction. It supersedes that idea for this problem: threading a batch builder across cores would buy 8–16x, whereas removing the rebuild buys 180x on the 420-holiday case.

**Measured result after implementation:**

| holidays | before (ms/leg) | after (ms/leg) | speedup |
|---|---|---|---|
| 0 | 0.045 | 0.036 | 1x |
| 120 | 9.63 | 0.107 | 90x |
| 420 | 43.6 | 0.242 | 180x |

Below the ~1000x originally predicted, because time still scales mildly with holiday count: `QCInterestRatePeriodsFactory` stores its calendar by value, so one `std::set` copy per leg remains. At 43 ms that copy was noise; at 0.24 ms it is most of what is left. See design for why by-value was nonetheless the right choice and what the upgrade path is.

## What Changes

- `QCBusinessCalendar` gains `businessDay(const QCDate&, QCDate::QCBusDayAdjRules) const` — the rule-dispatch `switch` currently living on `QCDate`, moved to the class that owns the holiday set and already implements every branch (`nextBusinessDay`, `previousBusinessDay`, `modNextBusinessDay`).
- `QCInterestRatePeriodsFactory` stops holding `shared_ptr<vector<QCDate>>` calendars and holds `QCBusinessCalendar` instead; its internal signatures take `const QCBusinessCalendar&`.
- `LegFactory` stops flattening calendars via `make_shared<vector<QCDate>>(cal.getHolidays())` and passes the `QCBusinessCalendar` it already has straight down.
- `QCDate` loses both `businessDay` overloads (moved to `QCBusinessCalendar`) and `addWeeks` (no live caller, no binding).
- `QCDate::shift` and `QCDate::monthDiffDayRemainder` stay on `QCDate` but take `const QCBusinessCalendar&` instead of a holiday vector, collapsing their `vector&`/`shared_ptr<vector>` overload pairs into one each. `shift` is **not** a duplicate of `QCBusinessCalendar::shift`: it carries `QCSettlementLagBehaviour` handling and takes direction as an adjustment rule rather than the sign of `nDays`.
- `QCFactoryFunctions.cpp` keeps its `vector<QCDate>` public signatures — its callers depend on them — and adapts locally via a file-scope `asCalendar` helper.
- Version bump in `setup.py`.

This is a pure performance change. No numerical result changes — verified byte-identical across 65 cases / 776 cashflow rows spanning every builder, adjustment rule, and stub convention.

## Capabilities

### Modified Capabilities

- `business-calendar`: gains rule-dispatched business-day adjustment as a calendar operation, and a requirement that adjustment must not reconstruct the holiday set.

<!-- front-stub-date-schedule is untouched: QCInterestRatePeriodsFactory changes representation only, not schedule logic -->

## Impact

- **Modified code**: `include/time/QCBusinessCalendar.h` + `source/time/QCBusinessCalendar.cpp` (new method); `include/time/QCDate.h` + `source/time/QCDate.cpp` (deletions, one signature change); `include/QCInterestRatePeriodsFactory.h` + `source/QCInterestRatePeriodsFactory.cpp` (member and parameter types); `source/LegFactory.cpp` (delete flattening, pass calendars); `source/QCFactoryFunctions.cpp` (local `asCalendar` adapter). `source/CMakeLists.txt` is unchanged.
- **Not deleted after all**: `QCFactoryFunctions` and `QCDiscountBondPayoff` were proposed for deletion on the basis that the active binder does not reference them. That was checked against the binder only. `QCFXForward.cpp` and `QCTimeDepositPayoff.cpp` — both in the live `target_sources` block — include `QCDiscountBondPayoff.h` and hold `shared_ptr<QCDiscountBondPayoff>` members, and `QCDiscountBondPayoff.cpp` includes `QCFactoryFunctions.h`. Deleting the pair means deleting that whole subtree, which is a separate and much larger decision. Both files stay and are adapted to the new API.
- **Not in the build, therefore not touched**: `source/QC_DVE_PYBIND.cpp`, `include/QCDvePyBindHelperFunctions.h`, and `source/QC_Financial.cpp` — the first two are unreferenced by any target, and `QC_FINANCIAL_SOURCES` (`source/CMakeLists.txt:80`) is a variable no target consumes. These hold 13 of the call sites a naive grep reports.
- **Public Python surface**: unchanged, but not for free. `QCDate::businessDay(vector&, rule)` is bound as `QCDate.business_day` (`source/qcf_binder.cpp:148`), so deleting the C++ method would break Python callers. It is preserved as a binder-level lambda that builds a `QCBusinessCalendar` from the passed list and delegates to the new method — same semantics, same (bad) performance for anyone calling it directly, and zero effect on the hot path, which no longer routes through it. `BusinessCalendar` additionally gains `business_day`.
- **Explicitly out**: caching `excelSerial()` inside `QCDate` (a further constant-factor win on `operator<`, separate change); the C++ batch operation builder; any change to `Portfolio` or its batch queries; making weekend days configurable.
