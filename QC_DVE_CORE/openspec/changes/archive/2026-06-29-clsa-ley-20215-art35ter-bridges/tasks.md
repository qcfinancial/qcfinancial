## 1. CLSA rules

- [x] 1.1 Add two `fixedOnWeekday` rules to `santiagoRules()` in `source/time/CalendarFactory.cpp`: `(9, 17, Monday, fromYear=2007)` and `(9, 20, Friday, fromYear=2007)`, with a comment citing Ley 20.215 Article 35 ter and the weekday equivalence

## 2. One-off table cleanup

- [x] 2.1 Remove from `chileOneOffHolidays()` the entries now produced by the rules: 2012-09-17, 2013-09-20, 2018-09-17, 2019-09-20, 2024-09-20
- [x] 2.2 Keep 2010-09-17, 2010-09-20 (bicentenario), 2017-04-19 (census), 2022-09-16 (plebiscite) and all solstice entries; update the explanatory comment to note the Sep-17-Mon / Sep-20-Fri bridges are now rules
- [x] 2.3 Related fix: gate the Oct 31 Día de las Iglesias Evangélicas rule with `fromYear = 2008` (Ley 20.299) — exposed by extending CLSA to 2007; removes a spurious 2007-11-02

## 3. Tests

- [x] 3.1 Add Art 35 ter tests: Sep 17 2029 present (Monday), Sep 20 2030 present (Friday), neither present in 2025, Sep 17 2001 absent (pre-2007), CLBA inherits (Sep 17 2029)

## 4. Build, validate, version

- [x] 4.1 Build the wheel and install into the test venv
- [x] 4.2 Cross-check CLSA vs python-holidays 0.99 over 2010–2050: require ZERO divergence in both directions (the prior ≥2026 divergence is now resolved)
- [x] 4.3 Bump `version=` in `setup.py` (1.11.1 → 1.11.2) and update the `id()` build string in `source/qcf_binder.cpp`

## 5. Commit

- [x] 5.1 Commit with message `# Update to Version 1.11.2: model Ley 20.215 Art 35 ter Fiestas Patrias sandwich bridges as rules (CLSA/CLBA)`
