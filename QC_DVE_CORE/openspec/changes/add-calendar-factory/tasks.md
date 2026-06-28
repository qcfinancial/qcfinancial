## 1. QCDate date primitives

- [x] 1.1 Add `QCDate::easterSunday(int year)` (Anonymous Gregorian computus) to `include/time/QCDate.h` + `source/time/QCDate.cpp`
- [x] 1.2 Add `QCDate::nthWeekdayOfMonth(int n, QCWeekDay weekday, int month, int year)` with `n < 0` counting from month end
- [x] 1.3 Add `QCDate::lastWeekdayOfMonth(QCWeekDay weekday, int month, int year)` (delegates to `nthWeekdayOfMonth(-1, ...)`)
- [x] 1.4 Add unit tests for the three primitives in `Tests/` (Easter table across several years incl. a century boundary; nth-from-start, nth-from-end / last-weekday)

## 2. BusinessCalendarId enum and string round-trip

- [x] 2.1 Add `enum class BusinessCalendarId { CLSA, USNY, USGS, EUTA }` (new header under `include/time/`)
- [x] 2.2 Implement `fpmlCode(BusinessCalendarId) -> std::string` and `description(BusinessCalendarId) -> std::string`
- [x] 2.3 Implement `fromFpmlCode(const std::string&) -> BusinessCalendarId` as the inverse of `fpmlCode`, throwing on unknown codes
- [x] 2.4 Add unit tests for round-trip (`fromFpmlCode(fpmlCode(id)) == id` for all ids), unknown-code throw, and non-empty descriptions

## 3. Rule engine

- [x] 3.1 Define the rule taxonomy: `FixedDate(month, day)`, `NthWeekdayOfMonth(n, weekday, month)`, `EasterRelative(offsetDays)`, `SpecialOneOff(date)` (new header under `include/time/`)
- [x] 3.2 Add per-rule validity window (`fromYear`/`toYear`) and observance policy fields, with a calendar-level default observance
- [x] 3.3 Implement rule resolution: given a rule + year → concrete `QCDate` (using the QCDate primitives from section 1)
- [x] 3.4 Implement observance application (`SatToFri|SunToMon`, `SunToMon`, `nearest`, `none`) on a resolved date
- [x] 3.5 Add unit tests for each rule kind, validity-window boundary years, and each observance policy (holiday on Sat and on Sun)

## 4. Calendar rule sets

- [x] 4.1 Define a private US-base ruleset (New Year, MLK 3rd Mon Jan, Washington's Birthday 3rd Mon Feb, Memorial last Mon May, Juneteenth Jun 19 `fromYear=2021`, Independence Jul 4, Labor 1st Mon Sep, Thanksgiving 4th Thu Nov, Christmas Dec 25) with US Sat→Fri/Sun→Mon default observance
- [x] 4.2 Define `USNY` = US base; define `USGS` = US base + Columbus Day (2nd Mon Oct) + Veterans Day (Nov 11)
- [x] 4.3 Define `CLSA` ruleset (Chilean fixed + Easter-relative holidays) with its observance policy
- [x] 4.4 Define `EUTA` ruleset (New Year, Good Friday, Easter Monday, Labour Day May 1, Christmas Dec 25, Boxing Day Dec 26) with no Sat/Sun observance shift
- [x] 4.5 Wire a `BusinessCalendarId -> ruleset` lookup

## 5. CalendarFactory

- [x] 5.1 Add `CalendarFactory::build(const QCDate& startDate, int nYears, <list of BusinessCalendarId>) -> QCBusinessCalendar` (new header + source under `include/time/` + `source/time/`)
- [x] 5.2 Implement generation: for each requested id, loop years `[startYear .. startYear+nYears]`, resolve rules, apply that calendar's observance, then union all resolved date sets into one `QCBusinessCalendar(startDate, nYears)`
- [x] 5.3 Handle the empty-list case (calendar with no holidays) and duplicate collapsing
- [x] 5.4 Add factory tests: single calendar, merged union (CLSA+USNY), empty list, and known-date checks per calendar incl. observance edge cases

## 6. Build wiring and Python bindings

- [x] 6.1 Register new `.cpp` files in `source/CMakeLists.txt` under `target_sources(QC_DVE_CORE ...)`
- [x] 6.2 Add a `registerCalendarFactory(py::module& m)` (and enum/round-trip) helper in `QcfinancialPybind11Helpers.h`; expose the enum via `.value(name, val, "description")` per-value docstrings and bind `fpmlCode`/`description`/`fromFpmlCode` and `CalendarFactory::build`
- [x] 6.3 Call the helper from `source/qcf_binder.cpp`
- [x] 6.4 Build the wheel (`python setup.py bdist_wheel`) and smoke-test from Python: build a CLSA+USNY calendar, check a known holiday and an observed-shift date, and read an enum value `__doc__`

## 7. Version bump and commit

- [x] 7.1 Bump `version=` in `setup.py`
- [x] 7.2 Commit with message `# Update to Version X.Y.Z: add CalendarFactory and BusinessCalendarId (CLSA/USNY/USGS/EUTA)`
