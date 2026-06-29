## Requirements

### Requirement: CalendarFactory builds a single merged calendar
`CalendarFactory::build(const QCDate& startDate, int nYears, <list of BusinessCalendarId>)` SHALL return one `QCBusinessCalendar` whose holiday set is the union of the holidays generated for every requested `BusinessCalendarId`, over the horizon `[startDate, startDate + nYears years]`. The returned calendar's start date SHALL be `startDate` and its length SHALL be `nYears`. The factory SHALL NOT modify the `QCBusinessCalendar` class behavior; it only populates the holiday set.

#### Scenario: Single calendar requested
- **WHEN** `build(2025-01-01, 5, {USNY})` is called
- **THEN** the result is a `QCBusinessCalendar` containing exactly the USNY holidays for years 2025–2030, with start date 2025-01-01 and length 5

#### Scenario: Multiple calendars merged by union
- **WHEN** `build(2025-01-01, 2, {CLSA, USNY})` is called
- **THEN** the result contains every date that is a holiday in CLSA OR in USNY over the horizon, with duplicates collapsed into a single set entry

#### Scenario: Empty calendar list
- **WHEN** `build(2025-01-01, 5, {})` is called with no calendar ids
- **THEN** the result is a `QCBusinessCalendar` with no holidays (weekends still apply), start date 2025-01-01, length 5

### Requirement: Observance is applied per-calendar before merging
The factory SHALL resolve each requested calendar's observed holiday dates using that calendar's own observance policy BEFORE unioning the resulting date sets. Observance SHALL NOT be applied to the merged set as a whole.

#### Scenario: US holiday on a Saturday shifts to Friday
- **WHEN** a USNY/USGS holiday's nominal date falls on a Saturday
- **THEN** the observed holiday is the preceding Friday

#### Scenario: US holiday on a Sunday shifts to Monday
- **WHEN** a USNY/USGS holiday's nominal date falls on a Sunday
- **THEN** the observed holiday is the following Monday

#### Scenario: TARGET holidays are not shifted
- **WHEN** a EUTA holiday's nominal date falls on a weekend
- **THEN** no observed weekday holiday is added (TARGET uses no Sat/Sun observance shift)

### Requirement: Holiday rule taxonomy
The rule engine SHALL support exactly these rule kinds: fixed date `(month, day)`; fixed date on a specific weekday `(month, day, weekday)` which is emitted only when that calendar date falls on `weekday` (no observance shift is applied); nth weekday of month `(n, weekday, month)` where `n < 0` counts from the end of the month (last weekday = `-1`); Easter-relative `(offsetDays)` measured from Easter Sunday; and one-off special date `(date)`. Each rule SHALL accept an optional validity window (`fromYear`, `toYear`) and an observance policy, with a calendar-level default observance policy.

#### Scenario: Nth weekday from start
- **WHEN** a rule is `NthWeekdayOfMonth(3, Monday, January)` for year 2025
- **THEN** it resolves to 2025-01-20 (third Monday of January)

#### Scenario: Nth weekday from end
- **WHEN** a rule is `NthWeekdayOfMonth(-1, Monday, May)` for year 2025
- **THEN** it resolves to 2025-05-26 (last Monday of May)

#### Scenario: Easter-relative rule
- **WHEN** a rule is `EasterRelative(-2)` (Good Friday) for year 2025
- **THEN** it resolves to 2025-04-18 (Easter Sunday 2025-04-20 minus 2 days)

#### Scenario: Validity window excludes years before fromYear
- **WHEN** a rule has `fromYear = 2021` (e.g. Juneteenth, June 19)
- **THEN** the holiday is generated for 2021 and later but NOT for 2020 or earlier

#### Scenario: Fixed-on-weekday rule fires only on the target weekday
- **WHEN** a rule is `FixedOnWeekday(September, 17, Friday)`
- **THEN** it resolves to September 17 in years where September 17 is a Friday, and resolves to nothing in years where it is any other weekday

### Requirement: Supported calendars and rule composition
The factory SHALL support `BusinessCalendarId` values `CLSA`, `CLBA`, `USNY`, `USGS`, and `EUTA`. `USNY` and `USGS` SHALL share a common US-base ruleset; `USGS` SHALL additionally include Columbus Day (2nd Monday of October) and Veterans Day (November 11). `CLBA` (Chile banking) SHALL reuse the full `CLSA` ruleset plus a fixed, non-movable December 31 (Feriado bancario).

#### Scenario: US base holidays present in both US calendars
- **WHEN** building either USNY or USGS for 2025
- **THEN** the calendar includes New Year's Day, Martin Luther King Jr. Day (3rd Mon Jan), Washington's Birthday (3rd Mon Feb), Memorial Day (last Mon May), Juneteenth (Jun 19), Independence Day (Jul 4), Labor Day (1st Mon Sep), Thanksgiving (4th Thu Nov), and Christmas (Dec 25), each with US Sat→Fri / Sun→Mon observance

#### Scenario: USGS adds bond-market-only holidays
- **WHEN** building USGS for 2025
- **THEN** the calendar additionally includes Columbus Day (2nd Mon Oct) and Veterans Day (Nov 11), which are NOT present in USNY

#### Scenario: EUTA holidays
- **WHEN** building EUTA for 2025
- **THEN** the calendar includes New Year's Day (Jan 1), Good Friday, Easter Monday, Labour Day (May 1), Christmas Day (Dec 25), and Boxing Day (Dec 26)

#### Scenario: CLBA is CLSA plus December 31
- **WHEN** building CLBA for 2025
- **THEN** the calendar includes every CLSA holiday for 2025 AND December 31 2025, and December 31 is not date-shifted

### Requirement: CLSA models Chilean movable-holiday laws
The CLSA calendar SHALL apply the Chilean movable-holiday statutes. Under Ley 20.215, *San Pedro y San Pablo* (Jun 29) and *Encuentro de Dos Mundos* (Oct 12) SHALL move to the Monday of the same week when they fall on Tuesday, Wednesday, or Thursday, and to the Monday of the following week when they fall on Friday. Under Ley 20.299, *Día de las Iglesias Evangélicas* (Oct 31) SHALL move to the Friday of the previous week when it falls on a Tuesday, and to the immediately following Friday when it falls on a Wednesday.

#### Scenario: San Pedro y San Pablo shifts to Monday
- **WHEN** building CLSA for 2023, where Jun 29 falls on a Thursday
- **THEN** the holiday is observed on Monday Jun 26 (and not on Jun 29)

#### Scenario: Encuentro de Dos Mundos on a Friday shifts to next Monday
- **WHEN** a year has Oct 12 falling on a Friday
- **THEN** the holiday is observed on the Monday of the following week

#### Scenario: Iglesias Evangélicas shifts to a Friday
- **WHEN** building CLSA for 2023, where Oct 31 falls on a Tuesday
- **THEN** the holiday is observed on Friday Oct 27 (the Friday of the previous week)

### Requirement: CLSA includes solstice-based and one-off national holidays
The CLSA calendar SHALL include the *Día Nacional de los Pueblos Indígenas* (Ley 21.357, from 2021), whose date tracks the June solstice, and the genuinely one-time national holidays declared by special laws that are NOT covered by a permanent rule (e.g. the 2010 bicentenario days, the 2017 census day, the 2022 plebiscite day, and the historical Fiestas Patrias bridge days declared by single-year laws). Because these are not expressible as recurring rules, they MAY be represented as a maintained date table. The table SHALL include only genuine historical instances (up to and including 2025) and SHALL NOT project speculative future bridge days that depend on not-yet-enacted laws; permanent statutory bridges (Ley 20.983) SHALL be expressed as rules instead.

#### Scenario: Indigenous Peoples' Day present with solstice date
- **WHEN** building CLSA covering 2021 and 2024
- **THEN** the calendar includes Jun 21 2021 and Jun 20 2024 for Día Nacional de los Pueblos Indígenas

#### Scenario: Genuine historical one-off present
- **WHEN** building CLSA covering 2022
- **THEN** the calendar includes the Sep 16 2022 one-off national holiday

#### Scenario: No speculative future bridge projected
- **WHEN** building CLSA covering a year ≥ 2026 whose only candidate bridge is a non-statutory Sep-17-Monday or Sep-20-Friday case
- **THEN** that bridge day is NOT added (only permanent statutory holidays and rules apply)

### Requirement: CLSA models Ley 20.983 Fiestas Patrias and New-Year bridges
The CLSA calendar SHALL implement Ley 20.983 (effective 2017) as rules: September 17 SHALL be a holiday in any year (≥ 2017) where September 18 falls on a Saturday and September 19 on a Sunday — equivalently, where September 17 falls on a Friday; and January 2 SHALL be a holiday in any year (≥ 2017) where January 1 falls on a Sunday — equivalently, where January 2 falls on a Monday. These holidays SHALL NOT be date-shifted. Because CLBA reuses the CLSA ruleset, CLBA SHALL inherit both rules.

#### Scenario: Fiestas Patrias bridge applies (Sep 17)
- **WHEN** building CLSA for 2021, where Sep 18 is a Saturday and Sep 19 a Sunday
- **THEN** the calendar includes September 17 2021 (a Friday)

#### Scenario: New-Year bridge applies (Jan 2)
- **WHEN** building CLSA for 2023, where Jan 1 is a Sunday
- **THEN** the calendar includes January 2 2023 (a Monday)

#### Scenario: Bridges do not apply when conditions are not met
- **WHEN** building CLSA for 2025 (Sep 17 is not a Friday) and 2024 (Jan 1 is not a Sunday)
- **THEN** the calendar does NOT include September 17 2025 and does NOT include January 2 2024

#### Scenario: CLBA inherits the Ley 20.983 rules
- **WHEN** building CLBA for 2021 and 2023
- **THEN** the calendar includes September 17 2021 and January 2 2023, in addition to the December 31 banking holiday

#### Scenario: Rules are inactive before 2017
- **WHEN** building CLSA for a pre-2017 year where Sep 17 falls on a Friday
- **THEN** the Ley 20.983 rule does NOT generate September 17 for that year (the law was not yet in force)

### Requirement: QCDate date primitives
`QCDate` SHALL provide `easterSunday(int year)` returning the Gregorian-computus Easter Sunday for the year, `nthWeekdayOfMonth(int n, QCWeekDay weekday, int month, int year)` returning the n-th occurrence (n < 0 counts from month end), and `lastWeekdayOfMonth(QCWeekDay weekday, int month, int year)` returning the last occurrence in the month.

#### Scenario: Easter Sunday computed by computus
- **WHEN** `QCDate::easterSunday(2025)` is called
- **THEN** it returns 2025-04-20

#### Scenario: Nth weekday of month
- **WHEN** `QCDate::nthWeekdayOfMonth(4, Thursday, November, 2025)` is called
- **THEN** it returns 2025-11-27 (4th Thursday of November = Thanksgiving)

#### Scenario: Last weekday of month
- **WHEN** `QCDate::lastWeekdayOfMonth(Monday, May, 2025)` is called
- **THEN** it returns 2025-05-26 (last Monday of May)

### Requirement: BusinessCalendarId FpML codes and string round-trip
The `BusinessCalendarId` enum symbols SHALL be the FpML business-center codes `CLSA`, `USNY`, `USGS`, `EUTA`, plus the house code `CLBA` (Chile banking; FpML has no canonical Chile-banking center). The system SHALL provide `fpmlCode(BusinessCalendarId)` returning the code string, `description(BusinessCalendarId)` returning a human-readable description, and `fromFpmlCode(const std::string&)` returning the matching enum value. `fromFpmlCode` SHALL be the inverse of `fpmlCode`.

#### Scenario: Code to enum and back
- **WHEN** `fromFpmlCode("CLSA")` is called
- **THEN** it returns `BusinessCalendarId::CLSA`, and `fpmlCode(BusinessCalendarId::CLSA)` returns `"CLSA"`

#### Scenario: Unknown code rejected
- **WHEN** `fromFpmlCode("ZZZZ")` is called with a code that maps to no calendar
- **THEN** the call signals an error (throws) rather than returning an arbitrary value

#### Scenario: Description available
- **WHEN** `description(BusinessCalendarId::EUTA)` is called
- **THEN** it returns a non-empty human-readable string identifying the TARGET / Eurozone calendar

### Requirement: Python bindings expose the factory and enum with docstrings
The `qcfinancial` pybind11 module SHALL expose `CalendarFactory::build`, the `BusinessCalendarId` enum, and the `fpmlCode` / `description` / `fromFpmlCode` functions. Each enum value SHALL carry a per-value docstring describing the calendar.

#### Scenario: Enum value docstring visible in Python
- **WHEN** a user inspects `qcfinancial.BusinessCalendarId.CLSA.__doc__` (or `help(qcfinancial.BusinessCalendarId)`)
- **THEN** a human-readable description of the Santiago calendar is shown

#### Scenario: Build callable from Python
- **WHEN** a user calls the bound `CalendarFactory.build` with a start date, year count, and a list of `BusinessCalendarId` values
- **THEN** a `QCBusinessCalendar` usable by existing pricing code is returned
