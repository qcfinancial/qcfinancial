## Why

Today a `QCBusinessCalendar` can only be built by manually inserting every holiday `QCDate` one by one. There is no way to say "give me a Santiago + New York calendar for the next 30 years" — callers must hardcode holiday lists, which is error-prone, drifts out of date, and gets the observance rules (e.g. US Saturday→Friday / Sunday→Monday shifts) wrong. Multi-currency valuation routinely needs a merged calendar (e.g. a CLP/USD swap needs Santiago ∪ New York), and there is no first-class way to produce one.

## What Changes

- Add a **`CalendarFactory`** with a single entry point that builds holidays from rules instead of by hand:
  `CalendarFactory::build(const QCDate& startDate, int nYears, <list of BusinessCalendarId>) -> QCBusinessCalendar`. It returns **one merged** calendar over the horizon `[startDate, startDate + nYears years]`, unioning the holidays of every requested calendar.
- Add a new **`BusinessCalendarId`** enum whose symbols are FpML business-center codes: `CLSA` (Santiago, Chile), `USNY` (US New York banking / SIFMA settlement), `USGS` (US government securities / SIFMA bond market), `EUTA` (Eurozone TARGET).
- Add a small **declarative holiday-rule engine** (rule taxonomy: fixed date, nth/last weekday of month, Easter-relative, one-off special date) with per-rule validity windows and observance policies. Rules are generated eagerly per year (python-holidays style) and observance is applied per-calendar **before** merging.
- Add three reusable date primitives to **`QCDate`**: `easterSunday(year)` (Gregorian computus), `nthWeekdayOfMonth(n, weekday, month, year)`, and `lastWeekdayOfMonth(weekday, month, year)`.
- Add **string round-trip + descriptions** for the enum: `fpmlCode(id)`, `description(id)`, `fromFpmlCode(code)`, and pybind11 per-value docstrings.
- Expose all of the above through the `qcfinancial` pybind11 module.
- `QCBusinessCalendar` itself is **unchanged** — the factory only fills its existing holiday set.
- **Out of scope (v1):** non-Sat/Sun weekends (Fri/Sat Middle-East calendars) and any calendar beyond the four listed above.

## Capabilities

### New Capabilities
- `calendar-factory`: rule-driven construction of a merged `QCBusinessCalendar` from a start date, a year count, and a set of `BusinessCalendarId` values; covers the rule taxonomy, per-calendar observance, the FpML enum and its string round-trip, and the supporting `QCDate` date primitives.

### Modified Capabilities
<!-- None. QCBusinessCalendar behavior is unchanged; QCDate gains new methods but no existing requirement changes (and neither has an existing spec). -->

## Impact

- **New code:** `BusinessCalendarId` enum + helpers, the rule engine, and `CalendarFactory` (new headers/sources under `include/time/` + `source/time/`); new `QCDate` methods.
- **Modified code:** `QCDate.h`/`QCDate.cpp` (new primitives), `source/CMakeLists.txt` (`target_sources`), `QcfinancialPybind11Helpers.h` + `source/qcf_binder.cpp` (bindings), `setup.py` (version bump).
- **Tests:** new test files for the `QCDate` primitives and the factory output (known holiday dates incl. observance edge cases) in `Tests/`.
- **Public API:** new pybind11 symbols (`BusinessCalendarId`, `CalendarFactory`, round-trip functions). Additive; no breaking changes.
- **References:** design draws on QuantLib calendar classes and the python-holidays library for rule and observance best practices. The `QC_DVE_CORE/qcfinancial` folder is reference-only and untouched.
