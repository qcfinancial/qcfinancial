## 1. Rule engine — fixedOnWeekday

- [x] 1.1 Add `RuleKind::fixedOnWeekday` to `include/time/HolidayRule.h`
- [x] 1.2 Add a `HolidayRule::fixedOnWeekday(month, day, weekday, fromYear, toYear)` factory helper (declaration + implementation in `source/time/HolidayRule.cpp`)
- [x] 1.3 Handle `fixedOnWeekday` in `HolidayRule::resolve`: build `QCDate(day, month, year)`, return it only if `weekDay() == weekday` (respecting the validity window), else `std::nullopt`; apply no observance shift. Comment the Ley 20.983 equivalence (Sep17=Fri ⟺ Sep18/19=Sat/Sun; Jan2=Mon ⟺ Jan1=Sun)

## 2. CLSA rules and one-off table

- [x] 2.1 Add two `fixedOnWeekday` rules to `santiagoRules()` in `source/time/CalendarFactory.cpp`: `(9, 17, Friday, fromYear=2017)` and `(1, 2, Monday, fromYear=2017)`
- [x] 2.2 In `chileOneOffHolidays()`, remove the Sep-17-Friday entries (2021, 2027, 2032, 2038, 2049) and all Jan-2 entries (2017, 2023, 2034, 2040, 2045) — now produced by the rules
- [x] 2.3 In `chileOneOffHolidays()`, remove the speculative future bridges (year ≥ 2026): Sep-17-Monday 2029, 2035, 2040, 2046 and Sep-20-Friday 2030, 2041, 2047
- [x] 2.4 Keep the genuine historical one-offs (year ≤ 2025): 2010-09-17, 2010-09-20, 2012-09-17, 2013-09-20, 2017-04-19, 2018-09-17, 2019-09-20, 2022-09-16, 2024-09-20; keep all Indigenous Peoples' Day solstice entries unchanged; update the explanatory comment

## 3. Tests

- [x] 3.1 Add a `fixedOnWeekday` unit test (fires on the target weekday year, empty otherwise; respects fromYear)
- [x] 3.2 Add CLSA Ley 20.983 tests: Sep 17 2021 present, Jan 2 2023 present, Sep 17 2025 absent, Jan 2 2024 absent
- [x] 3.3 Add a CLBA inheritance test (Sep 17 2021 and Jan 2 2023 present alongside Dec 31)

## 4. Build, validate, version

- [x] 4.1 Build the wheel (`python setup.py bdist_wheel`) and install into the test venv
- [x] 4.2 Re-run the CLSA cross-check vs python-holidays 0.99: require parity for years ≤ 2025; confirm the only divergences for ≥ 2026 are the intentionally-dropped Sep-17-Monday / Sep-20-Friday bridges (Ley 20.983 Sep-17-Friday and Jan-2 cases still match)
- [x] 4.3 Bump `version=` in `setup.py` (1.11.0 → 1.11.1) and update the `id()` build string in `source/qcf_binder.cpp`

## 5. Commit

- [x] 5.1 Commit with message `# Update to Version 1.11.1: model Ley 20.983 (Fiestas Patrias / New-Year bridges) as a rule for CLSA/CLBA`
