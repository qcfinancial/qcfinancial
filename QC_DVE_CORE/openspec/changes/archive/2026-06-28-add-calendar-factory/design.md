## Context

`QCBusinessCalendar` (`include/time/QCBusinessCalendar.h`) is a materialized `std::set<QCDate>` of holidays plus a hardcoded Sat/Sun weekend, queried via `binary_search` in `nextBusinessDay` / `previousBusinessDay` / `shift`. Holidays today are inserted one at a time by the caller; there is no rule-driven construction. `QCDate` provides `weekDay()`, `addDays/addMonths`, `moveToDayOfMonth`, `isEndOfMonth`, and `excelSerial`, but has no nth-weekday, last-weekday, or Easter primitive.

Two mature references inform the design:
- **QuantLib** — *declarative* per-calendar rule definitions, evaluated lazily in `isBusinessDay(date)` with an `easterMonday(year)` table.
- **python-holidays** — *generative* `_populate(year)` that emits concrete `{date: name}` per year, with a first-class `observed` framework, validity windows, and one-off `special_holidays`.

`QCBusinessCalendar` is a materialized set over a finite horizon — the python-holidays *generative* model maps onto it directly, while QuantLib's *declarative* rule tables give the cleanest way to author the rules. This change combines the two.

## Goals / Non-Goals

**Goals:**
- One-call construction of a single **merged** `QCBusinessCalendar` from `(startDate, nYears, {BusinessCalendarId...})`.
- A small, readable, declarative rule taxonomy that covers CLSA, CLBA, USNY, USGS, EUTA.
- Correct per-calendar observance (notably US Sat→Fri / Sun→Mon), applied **before** merging.
- Reusable, independently testable `QCDate` date primitives (`easterSunday`, `nthWeekdayOfMonth`, `lastWeekdayOfMonth`).
- FpML-coded enum with string round-trip and self-documenting pybind11 docstrings.
- Leave `QCBusinessCalendar` untouched.

**Non-Goals:**
- Non-Sat/Sun weekends (Fri/Sat Middle-East calendars) — punted.
- Any calendar beyond the five listed.
- Lazy / infinite-horizon evaluation (QuantLib style) — the materialized-set model is kept.
- Data-driven rules loaded from external files — rules are hardcoded C++ tables in v1.

## Decisions

### D1. Generate eagerly per year, then merge (python-holidays model)
For each requested `BusinessCalendarId`, loop years `[startDate.year() .. startDate.year() + nYears]`, apply that calendar's rules to produce concrete `QCDate`s, apply that calendar's observance, then **union** all calendars' resolved date sets into one `QCBusinessCalendar(startDate, nYears)`.

*Why:* matches the existing materialized-set container exactly; no change to `QCBusinessCalendar`; output is enumerable and testable.
*Alternative rejected:* QuantLib-style lazy `isBusinessDay` would require reworking `QCBusinessCalendar`'s storage and lookups.

### D2. Observance is applied per-calendar, before the merge
Each ruleset resolves its own observed dates (US shifts Sat→Fri/Sun→Mon; Chile and TARGET differ) and only then are the concrete sets unioned.

*Why:* observance rules are calendar-specific; shifting after merging would apply the wrong policy to the wrong holidays and could collide distinct holidays. This ordering is a hard requirement, not an optimization.

### D3. Sealed rule taxonomy with cross-cutting fields
Four rule kinds: `FixedDate(month, day)`; `NthWeekdayOfMonth(n, weekday, month)` with `n < 0` counting from month end (last Monday = `-1`); `EasterRelative(offsetDays)`; `SpecialOneOff(date)`. Every rule carries an optional **validity window** (`fromYear`/`toYear`) and an **observance policy** (`SatToFri|SunToMon`, `SunToMon`, `nearest`, `none`) with a calendar-level default.

*Why:* this is the minimal set that expresses all four calendars and is the common core of both reference libraries. Validity windows are needed for real rules (Juneteenth `fromYear=2021`, Chile's Jan-2 bridge `fromYear=2017`). Putting observance on the rule (with a calendar default) follows python-holidays and keeps definitions readable, unlike QuantLib's interleaved boolean.

### D4. Internal composition to keep rule tables DRY
`USNY` and `USGS` share a private US-base ruleset (New Year, MLK 3rd Mon Jan, Washington's Birthday 3rd Mon Feb, Memorial last Mon May, Juneteenth Jun 19 `fromYear=2021`, Independence Jul 4, Labor 1st Mon Sep, Thanksgiving 4th Thu Nov, Christmas Dec 25). `USGS` additionally adds Columbus Day (2nd Mon Oct) and Veterans Day (Nov 11).

*Why:* avoids duplicating the US base and keeps the two US calendars in sync.

### D5. Easter via Gregorian computus on QCDate
Add `QCDate::easterSunday(int year)` implementing the Anonymous Gregorian (computus) algorithm; `EasterRelative` offsets from it (Good Friday `-2`, Easter Monday `+1`).

*Why:* exact, unbounded year range, no maintenance — superior to QuantLib's fixed lookup table for an arbitrary `nYears` horizon. Lives on `QCDate` so it is reusable and unit-testable independent of the factory.

### D6. FpML enum codes with string round-trip and docstrings
`enum class BusinessCalendarId { CLSA, CLBA, USNY, USGS, EUTA }`. Free functions `fpmlCode(id) -> string`, `description(id) -> string`, `fromFpmlCode(const string&) -> BusinessCalendarId`. In pybind11, expose via `.value("CLSA", BusinessCalendarId::CLSA, "Santiago, Chile — public holidays")` etc. (third arg is a per-value docstring), plus the round-trip functions. `CLBA` (Chile banking) is a house code — FpML defines no canonical Chile-banking center; it reuses the CLSA ruleset plus a non-movable Dec 31 (Feriado bancario).

*Why:* FpML business-center codes are the financial-industry standard (trade data and config speak them), the docstrings keep the terse codes self-documenting in Python `help()`, and `fromFpmlCode` lets callers pass strings from config/data without a C++ symbol.

### D8. Chilean movable-holiday laws and one-off / solstice holidays
CLSA models the Chilean movable-holiday statutes via two new observance policies on the affected fixed dates: `chileMondayShift` (Ley 20.215 — Jun 29 / Oct 12 slide to Monday) and `chileReformationShift` (Ley 20.299 — Oct 31 slides to a Friday). The solstice-based *Día Nacional de los Pueblos Indígenas* (Ley 21.357) and the ad-hoc one-off national holidays (Fiestas Patrias bridges, plebiscites, New-Year bridges, the 2017 census) are represented as a hardcoded `SpecialOneOff` date table sourced from the `holidays` Python library (v0.99), covering 2010–2050.

*Why:* the movable laws fit the existing observance abstraction (nominal date → observed date) with no taxonomy change. The solstice/ad-hoc dates are not expressible as recurring rules — the indigenous holiday tracks the astronomical June solstice and the rest are declared year-by-year — so a maintained table (matching the reference library) is the honest representation. The full CLSA output was cross-validated against `holidays` 0.99 with **zero discrepancies in both directions over 2018–2030**.
*Trade-off:* the one-off/solstice table needs extension beyond 2050 (same finite-horizon limitation as the reference library); flagged in a code comment.
*Note:* this refines the earlier "movable law out of scope" v1 simplification — it is now in scope. The hardcoded table is consistent with the "no external data files" non-goal (it is compiled-in C++, not loaded at runtime).

### D7. New code placement
New headers/sources under `include/time/` + `source/time/` for the enum/helpers, rule engine, and `CalendarFactory`. `QCDate` primitives added to the existing `QCDate.{h,cpp}`. Build/bindings wired per the CLAUDE.md Adding-a-New-Feature checklist: register `.cpp`s in `source/CMakeLists.txt`, add a pybind helper in `QcfinancialPybind11Helpers.h`, call it from `source/qcf_binder.cpp`, bump `setup.py` version.

## Risks / Trade-offs

- **Holiday-data accuracy** → Validate generated dates against known-good references (QuantLib output / python-holidays) in tests, including observance edge cases (holiday on Sat and on Sun) and validity-window boundary years (2020 vs 2021 for Juneteenth).
- **Observance ordering bug** → If observance were applied after merge it would be subtly wrong; D2 makes per-calendar-before-merge a tested invariant.
- **`n < 0` (from-end) indexing off-by-one** → `lastWeekdayOfMonth` / `nthWeekdayOfMonth(n<0)` get dedicated unit tests independent of the factory.
- **Computus correctness** → `easterSunday` tested against a table of known Easter Sundays across several years (including a century-boundary year).
- **Finite horizon** → Unlike QuantLib, the calendar only covers `[startDate, startDate + nYears]`; callers must request enough years. Documented; acceptable given the existing materialized-set model.
- **Merged-only return** → Callers cannot recover an individual calendar from the merged result; if a single-calendar build is later needed it is just `build(start, n, {oneId})`. No separate per-id return planned for v1.

## Open Questions

None outstanding — all design forks (SIFMA naming, merged return, computus, FpML/ISO codes, weekend punt, string round-trip + docstrings) were resolved during the explore session.
