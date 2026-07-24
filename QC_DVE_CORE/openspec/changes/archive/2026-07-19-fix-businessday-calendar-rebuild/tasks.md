## 0. Capture the baseline

- [x] 0.1 Record `Leg.record()` output for a reference set of legs — bullet and custom-amort, fixed rate / Ibor / overnight index / ICP-CLP, every adjustment rule and stub convention — and save it as the golden file (65 cases, 776 rows)
- [x] 0.2 Re-run the holiday-count sweep benchmark on the current build and record ms/leg at 0 / 120 / 420 holidays (0.045 / 9.63 / 43.6)

## 1. Calendar gains the adjustment

- [x] 1.1 In `include/time/QCBusinessCalendar.h`, declare `QCDate businessDay(const QCDate& fecha, QCDate::QCBusDayAdjRules rule) const`
- [x] 1.2 In `source/time/QCBusinessCalendar.cpp`, implement it as the `switch` from `QCDate::businessDay`, returning the input unchanged for `qcNo` and preserving that `qcModPrev` falls through to `previousBusinessDay`
- [x] 1.3 Make `nextBusinessDay`, `previousBusinessDay`, `modNextBusinessDay` and `shift` `const`

## 2. Periods factory holds a calendar

- [x] 2.1 In `include/QCInterestRatePeriodsFactory.h`, change `_settlementCalendar` and `_fixingCalendar` to `QCBusinessCalendar` (by value — see design)
- [x] 2.2 Change the corresponding constructor parameters to `const QCBusinessCalendar&`
- [x] 2.3 Change `_buildBasicDates`, `_buildBasicDates2`, `_auxWithLongFrontNEasyCase` and `_auxWithLongFrontNNotEasyCase` to `const QCBusinessCalendar&`
- [x] 2.4 In `source/QCInterestRatePeriodsFactory.cpp`, rewrite every `X.businessDay(calendar, rule)` as `calendar.businessDay(X, rule)` — 26 single-line plus 7 multiline sites
- [x] 2.5 `monthDiffDayRemainder` and `shift` call sites needed no edit: only the parameter type changed

## 3. LegFactory stops flattening

- [x] 3.1 Rebind the 16 `settCal`/`fixCal` aliases from `make_shared<vector<QCDate>>(cal.getHolidays())` to `const auto& settCal = settlementCalendar;` — keeps every downstream use working with no further edits
- [x] 3.2 Rewrite the four direct calls as `fixCal.businessDay(accrualStartDate, indexDateAdjustment)` and so on
- [x] 3.3 Confirm no `getHolidays()` call remains in `source/LegFactory.cpp`

## 4. QCDate sheds the calendar overloads

- [x] 4.1 Delete both `businessDay` overloads from `include/time/QCDate.h` and `source/time/QCDate.cpp`; add a `class QCBusinessCalendar;` forward declaration (the calendar header includes `QCDate.h`, so a real include would be circular)
- [x] 4.2 **Corrected during apply.** The original task said to delete `QCDate::shift` as a duplicate of `QCBusinessCalendar::shift`. It is not one — it handles `QCSettlementLagBehaviour` and takes direction as an adjustment rule rather than the sign of `nDays`, so `calendar.shift(d, 0)` and `d.shift(cal, 0, qcFollow, qcMoveToWorkingDay)` disagree. Kept, with its two overloads collapsed into one taking `const QCBusinessCalendar&`
- [x] 4.3 Delete `addWeeks(vector<QCDate>&, …)`; its only caller is the unbuilt `QCDvePyBindHelperFunctions.h` and it has no binding
- [x] 4.4 Collapse both `monthDiffDayRemainder` overloads into one taking `const QCBusinessCalendar&`
- [x] 4.5 Confirm no live file still calls the removed overloads

## 5. Dead translation units

- [x] 5.1 **Corrected during apply — nothing was dropped.** `QCFactoryFunctions.cpp` and `QCDiscountBondPayoff.cpp` were to be deleted as unreferenced. They are unreferenced *by the binder*, but `QCFXForward.cpp` and `QCTimeDepositPayoff.cpp` (both in the live `target_sources` block) include `QCDiscountBondPayoff.h` and hold `shared_ptr<QCDiscountBondPayoff>` members, and `QCDiscountBondPayoff.cpp` includes `QCFactoryFunctions.h`. Deleting the pair means deleting that whole subtree — a separate decision. `source/CMakeLists.txt` is unchanged
- [x] 5.2 Adapt `source/QCFactoryFunctions.cpp` to the new API instead: file-scope `asCalendar(const std::vector<QCDate>&)` helper, applied at 13 adjustment sites and 20 periods-factory calendar arguments, leaving its public `vector<QCDate>` signatures intact

## 6. Bindings

- [x] 6.1 Replace the `business_day` binding with the shim lambda — builds a `QCBusinessCalendar` from the passed list, delegates, keeps the Python signature identical
- [x] 6.2 Add `.def("business_day", &QCBusinessCalendar::businessDay)` to `BusinessCalendar`
- [x] 6.3 Confirm no other binding references a deleted overload

## 7. Verify

- [x] 7.1 Build the wheel for the current pyenv version — clean
- [x] 7.2 Golden diff: **byte-identical across all 65 cases / 776 rows**, no key added or dropped
- [x] 7.3 Benchmark: 0.045 → 0.036 (0 hol), 9.63 → 0.107 (120 hol, 90x), 43.6 → 0.242 (420 hol, **180x**). Below the predicted ~1000x; the residual is the per-leg calendar copy from by-value storage, documented in design
- [x] 7.4 Python surface: `QCDate.business_day(holiday_list, rule)` unchanged and agrees with `BusinessCalendar.business_day(date, rule)` across every rule on holiday, weekend and month-end inputs
- [x] 7.5 `./compile.sh` — all four supported pyenv versions (3.11.15, 3.12.13, 3.13.13, 3.14.5) build clean

## 8. Release

- [x] 8.1 Bump `version=` in `setup.py`
- [x] 8.2 Update the version line in `CLAUDE.md`
- [x] 8.3 Commit as `# Update to Version X.Y.Z: <description>` (`8b67db1`), plus `6d9f766` for the binder `id()` string missed in 8.1/8.2
