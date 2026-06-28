## ADDED Requirements

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
The rule engine SHALL support exactly these rule kinds: fixed date `(month, day)`; nth weekday of month `(n, weekday, month)` where `n < 0` counts from the end of the month (last weekday = `-1`); Easter-relative `(offsetDays)` measured from Easter Sunday; and one-off special date `(date)`. Each rule SHALL accept an optional validity window (`fromYear`, `toYear`) and an observance policy, with a calendar-level default observance policy.

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

### Requirement: Supported calendars and rule composition
The factory SHALL support `BusinessCalendarId` values `CLSA`, `USNY`, `USGS`, and `EUTA`. `USNY` and `USGS` SHALL share a common US-base ruleset; `USGS` SHALL additionally include Columbus Day (2nd Monday of October) and Veterans Day (November 11).

#### Scenario: US base holidays present in both US calendars
- **WHEN** building either USNY or USGS for 2025
- **THEN** the calendar includes New Year's Day, Martin Luther King Jr. Day (3rd Mon Jan), Washington's Birthday (3rd Mon Feb), Memorial Day (last Mon May), Juneteenth (Jun 19), Independence Day (Jul 4), Labor Day (1st Mon Sep), Thanksgiving (4th Thu Nov), and Christmas (Dec 25), each with US Sat→Fri / Sun→Mon observance

#### Scenario: USGS adds bond-market-only holidays
- **WHEN** building USGS for 2025
- **THEN** the calendar additionally includes Columbus Day (2nd Mon Oct) and Veterans Day (Nov 11), which are NOT present in USNY

#### Scenario: EUTA holidays
- **WHEN** building EUTA for 2025
- **THEN** the calendar includes New Year's Day (Jan 1), Good Friday, Easter Monday, Labour Day (May 1), Christmas Day (Dec 25), and Boxing Day (Dec 26)

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
The CLSA calendar SHALL include the *Día Nacional de los Pueblos Indígenas* (Ley 21.357, from 2021), whose date tracks the June solstice, and the ad-hoc one-off national holidays declared by special laws (Fiestas Patrias bridge days, plebiscite days, New-Year bridge days, the 2017 census day). Because these are not expressible as simple recurring rules, they MAY be represented as a maintained date table; the table SHALL match the reference `holidays` Python library over its covered horizon.

#### Scenario: Indigenous Peoples' Day present with solstice date
- **WHEN** building CLSA covering 2021 and 2024
- **THEN** the calendar includes Jun 21 2021 and Jun 20 2024 for Día Nacional de los Pueblos Indígenas

#### Scenario: One-off national holiday present
- **WHEN** building CLSA covering 2022
- **THEN** the calendar includes the Sep 16 2022 one-off national holiday

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
The `BusinessCalendarId` enum symbols SHALL be the FpML business-center codes `CLSA`, `USNY`, `USGS`, `EUTA`. The system SHALL provide `fpmlCode(BusinessCalendarId)` returning the code string, `description(BusinessCalendarId)` returning a human-readable description, and `fromFpmlCode(const std::string&)` returning the matching enum value. `fromFpmlCode` SHALL be the inverse of `fpmlCode`.

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
