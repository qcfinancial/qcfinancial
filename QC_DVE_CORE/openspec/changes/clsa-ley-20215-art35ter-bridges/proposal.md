## Why

The `clsa-ley-20983-bridge-rule` change treated the Chilean "Fiestas Patrias sandwich" bridges (Sep 17 when Sep 18 = Tuesday; Sep 20 when Sep 19 = Thursday) as one-time laws, kept only their historical instances (≤ 2025), and deliberately stopped projecting them into the future. That was wrong: these bridges are **permanent law** — Article 35 ter of Ley 20.215 — so they should be modeled as a rule, not a finite table, and should apply to all years from the law's effect.

## What Changes

- Model **Ley 20.215, Article 35 ter** on CLSA as two rules (effective `fromYear = 2007`), reusing the existing `fixedOnWeekday` rule kind:
  - Sep 17 is a holiday when Sep 18 = Tuesday and Sep 19 = Wednesday — equivalently when **Sep 17 falls on a Monday** → `fixedOnWeekday(9, 17, Monday, 2007)`.
  - Sep 20 is a holiday when Sep 18 = Wednesday and Sep 19 = Thursday — equivalently when **Sep 20 falls on a Friday** → `fixedOnWeekday(9, 20, Friday, 2007)`.
- **Remove** the historical one-off entries now produced by these rules: 2012-09-17, 2013-09-20, 2018-09-17, 2019-09-20, 2024-09-20.
- **Keep** the remaining genuine one-offs not covered by any rule: 2010-09-17 and 2010-09-20 (bicentenario), 2017-04-19 (census), 2022-09-16 (plebiscite); and all Día de los Pueblos Indígenas solstice entries.
- CLBA inherits both rules automatically (it reuses the CLSA ruleset).
- Bump version (1.11.1 → 1.11.2) and add tests.

This **supersedes** the prior change's "drop speculative future bridges (≥ 2026)" decision. With `fromYear = 2007`, the rules reproduce python-holidays' Sep-17-Monday and Sep-20-Friday dates exactly and restore **full CLSA parity** with the reference library across 2010–2050 (and additionally generate the historically-correct 2007-09-17 that the table omitted).

## Capabilities

### New Capabilities
<!-- None. -->

### Modified Capabilities
- `calendar-factory`: add a requirement specifying Ley 20.215 Article 35 ter (Sep-17-Monday / Sep-20-Friday bridges) as rules; update the CLSA solstice/one-off requirement to drop the historical bridge entries now covered by the rule and to remove the "no speculative future bridge" constraint (those bridges are permanent law and are now projected via rule).

## Impact

- **Modified code:** `source/time/CalendarFactory.cpp` (two `fixedOnWeekday` rules added to `santiagoRules()`; five entries removed from `chileOneOffHolidays()`); `setup.py` (version); `source/qcf_binder.cpp` (`id()` build string). No `HolidayRule` change needed — `fixedOnWeekday` already exists.
- **Tests:** `Tests/CalendarFactoryTests.cpp` (Art 35 ter scenarios + CLBA inheritance).
- **Behavior:** CLSA/CLBA now include the Sep-17-Monday and Sep-20-Friday bridges for all qualifying years ≥ 2007 (not just ≤ 2025). Restores full parity with python-holidays 0.99.
- **Spec:** delta against `openspec/specs/calendar-factory/spec.md`.
- **Reference:** Ley 20.215, Article 35 ter (Chile).
