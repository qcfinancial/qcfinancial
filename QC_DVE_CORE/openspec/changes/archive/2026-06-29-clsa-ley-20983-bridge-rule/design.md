## Context

CLSA's Chilean holidays were validated to full parity with the python `holidays` library in the `add-calendar-factory` change. That parity was partly achieved by embedding a `SpecialOneOff` table (`chileOneOffHolidays()` in `source/time/CalendarFactory.cpp`) of Fiestas Patrias bridge days, New-Year bridge days, the solstice-based Indigenous Peoples' Day, and assorted one-offs — copied from python-holidays.

Most of the September and January bridge entries are not genuinely ad-hoc: they follow **Ley 20.983** (published 2016, effective 2017), which permanently declares Sep 17 a holiday when Sep 18/19 fall on Saturday/Sunday, and Jan 2 a holiday when Jan 1 falls on Sunday. The rule engine (`include/time/HolidayRule.h`) already supports `fixedDate`, `nthWeekdayOfMonth`, `easterRelative`, and `specialOneOff` kinds with validity windows and observance policies, but none expresses a *weekday-conditional* fixed date.

## Goals / Non-Goals

**Goals:**
- Express Ley 20.983 as a permanent rule (correct for all years ≥ 2017), removing the corresponding hardcoded table rows.
- Keep CLSA historically accurate (≤ 2025) while not inventing future, not-yet-enacted bridge holidays.
- Apply automatically to CLBA (which reuses the CLSA ruleset).

**Non-Goals:**
- Converting the solstice-based Indigenous Peoples' Day to a rule — it tracks the astronomical June solstice and stays a table (separate law, out of scope here).
- Matching python-holidays' *speculative* future projections of non-statutory bridges.
- Any change to USNY/USGS/EUTA.

## Decisions

### D1. New rule kind `fixedOnWeekday`
Add `RuleKind::fixedOnWeekday`: emit `(month, day)` for a given year **only if** `QCDate(day, month, year).weekDay() == requiredWeekday`; otherwise emit nothing. No observance shift is applied (the date is, by construction, already on the target weekday). Reuse the existing `month`, `day`, and `weekday` fields; add a `HolidayRule::fixedOnWeekday(month, day, weekday, fromYear, toYear)` factory helper and a `resolve()` branch.

*Why:* this is the minimal, general primitive that expresses Ley 20.983. The law's two clauses collapse to single-weekday conditions:
- *Sep 18 = Sat ∧ Sep 19 = Sun* ⟺ *Sep 17 = Friday* → `fixedOnWeekday(9, 17, Friday)`
- *Jan 1 = Sunday* ⟺ *Jan 2 = Monday* → `fixedOnWeekday(1, 2, Monday)`

*Alternative rejected:* a bespoke "conditional on a reference date's weekday" rule (carry refMonth/refDay/refWeekday). More fields, no added expressiveness for this law — the emitted date's own weekday is sufficient and simpler. The equivalence is documented in code comments.

### D2. Effective from 2017
Both rules carry `fromYear = 2017` (Ley 20.983's first applicable year). Pre-2017 Sep-17 / Jan-2 holidays, where they existed, were ad-hoc one-time laws and remain (or not) as `specialOneOff` entries — the rule does not retroactively manufacture them.

### D3. Keep historical one-offs, drop future speculation
The non-Ley-20.983 September bridges (Sep-17-Monday when Sep 18 = Tuesday; Sep-20-Friday when Sep 19 = Thursday) originate from separate single-year laws. Keep the real historical instances (year ≤ 2025) as `specialOneOff`; remove the entries python-holidays projects for year ≥ 2026.

*Why:* those future dates are not yet enacted law — projecting them is a guess. Keeping history while not inventing the future is the more correct contract. This is a deliberate, documented divergence from python-holidays for ≥ 2026.

### D4. CLBA inherits automatically
`chileBankingRules()` is `santiagoRules()` + Dec 31, so adding the rules to `santiagoRules()` covers both CLSA and CLBA with no extra wiring.

## Risks / Trade-offs

- **Behavior change for existing years** → CLSA/CLBA holiday sets shift (Sep 17 / Jan 2 now rule-driven; future non-statutory bridges removed). Mitigation: cross-check vs python-holidays 0.99 — require parity for ≤ 2025 and only the *expected* divergences for ≥ 2026; add explicit tests (Sep 17 2021 present, Jan 2 2023 present, 2025 has no Sep 17, 2024 has no Jan 2).
- **`fixedOnWeekday` with no observance** → correct here, but a future caller could combine it with an observance expecting a shift. Mitigation: document that the rule's emitted date is already weekday-pinned and observance is not applied.
- **Pre-2017 accuracy** → the rule won't generate pre-2017 Sep-17/Jan-2; acceptable since the law did not exist then and the calendar is forward-looking.

## Open Questions

None — law scope (full Ley 20.983: Sep 17 + Jan 2) and the historical-vs-future bridge policy (keep ≤ 2025, drop ≥ 2026) were decided before drafting.
