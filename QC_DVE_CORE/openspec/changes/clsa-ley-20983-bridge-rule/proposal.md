## Why

The CLSA (and CLBA) calendar currently models Chile's Fiestas Patrias bridge and New-Year bridge holidays as a hand-maintained `SpecialOneOff` date table copied from the python `holidays` library. Most of those entries are not ad-hoc at all — they are the deterministic consequence of **Ley 20.983** (2016): *Sep 17 is a holiday when Sep 18/19 fall on Saturday/Sunday*, and *Jan 2 is a holiday when Jan 1 falls on Sunday*. Encoding a permanent statute as a frozen table is fragile (it silently expires past the table's horizon) and conflates the law with genuinely one-time holidays. A rule is correct for all years and removes the maintenance burden.

## What Changes

- Add a new rule kind **`fixedOnWeekday`** to the holiday-rule engine: emit a fixed `(month, day)` only when that date falls on a specified weekday (no observance shift). This expresses Ley 20.983 exactly, since *Sep 18/19 = Sat/Sun* ⟺ *Sep 17 is a Friday*, and *Jan 1 = Sunday* ⟺ *Jan 2 is a Monday*.
- Model **Ley 20.983** on CLSA via two `fixedOnWeekday` rules (effective `fromYear = 2017`): Sep 17 when Friday, and Jan 2 when Monday. CLBA inherits these automatically (it reuses the CLSA ruleset).
- **Remove** from the CLSA one-off table the entries now produced by the rule: the Sep-17-Friday dates (2021, 2027, 2032, 2038, 2049) and all Jan-2 dates (2017, 2023, 2034, 2040, 2045).
- **Remove** the speculative future projections (year ≥ 2026) of the *other* September bridges that are not Ley 20.983 (the Sep-17-Monday and Sep-20-Friday cases, which historically came from separate one-time laws). **Keep** the genuine historical ones (year ≤ 2025) as one-offs.
- **Keep unchanged** the solstice-based *Día Nacional de los Pueblos Indígenas* entries (Ley 21.357) and the remaining genuine one-offs (2010 bicentenario, 2017 census, 2022 plebiscite).
- Bump the version (1.11.0 → 1.11.1) and add tests.

This intentionally diverges from python-holidays for years ≥ 2026, where python-holidays projects future Sep-17-Monday / Sep-20-Friday bridges that are not yet enacted law. The Ley 20.983 cases (Sep-17-Friday, Jan-2) remain in full agreement.

## Capabilities

### New Capabilities
<!-- None. -->

### Modified Capabilities
- `calendar-factory`: the holiday-rule taxonomy gains the `fixedOnWeekday` rule kind; the CLSA "solstice-based and one-off national holidays" requirement is narrowed (Ley 20.983 is now a rule, not table entries; no speculative future bridges); and a new requirement specifies Ley 20.983 behavior for CLSA/CLBA.

## Impact

- **Modified code:** `include/time/HolidayRule.h` + `source/time/HolidayRule.cpp` (new `RuleKind::fixedOnWeekday`, factory helper, `resolve` handling); `source/time/CalendarFactory.cpp` (two new CLSA rules + edited `chileOneOffHolidays()`); `setup.py` (version).
- **Tests:** `Tests/CalendarFactoryTests.cpp` (fixedOnWeekday + Ley 20.983 scenarios).
- **Behavior:** CLSA and CLBA holiday sets change for some years (Sep 17 / Jan 2 now rule-driven; future non-statutory bridges dropped ≥ 2026). Additive `fixedOnWeekday` rule kind; no breaking API changes.
- **Spec:** delta against `openspec/specs/calendar-factory/spec.md`.
- **Reference:** Ley 20.983 (Biblioteca del Congreso Nacional, `idNorma=1098384`).
