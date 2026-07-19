## Purpose

Holiday calendars and the business-day arithmetic built on them: adjusting a date to a business day under a given convention, shifting by a number of business days, and merging calendars across jurisdictions. Adjustment is an operation of the calendar, since it requires the holiday set.
## Requirements
### Requirement: Holiday-aware business-day adjustment
`nextBusinessDay` SHALL return the input date if it is a business day, otherwise the nearest following date that is neither a Saturday, a Sunday, nor a member of the holiday set. `previousBusinessDay` SHALL do the same searching backward. Holiday membership SHALL be tested using the holiday set's own logarithmic-time lookup (`std::set::find`/`count`), not a linear scan. The results SHALL be identical to the previous implementation.

#### Scenario: Weekday holiday is skipped forward
- **WHEN** the input date is a holiday that falls on a Wednesday
- **THEN** `nextBusinessDay` returns the following Thursday (assuming it is a business day)

#### Scenario: Friday holiday rolls to Monday
- **WHEN** the input date is a holiday that falls on a Friday
- **THEN** `nextBusinessDay` returns the following Monday (skipping the weekend)

#### Scenario: Monday holiday rolls back to Friday
- **WHEN** the input date is a holiday that falls on a Monday
- **THEN** `previousBusinessDay` returns the preceding Friday

#### Scenario: Consecutive holidays are all skipped
- **WHEN** several consecutive dates (including across a weekend) are all holidays
- **THEN** `nextBusinessDay` returns the first business day after the entire run

### Requirement: Calendar merge covers both calendars
`operator+` SHALL return a calendar whose holiday set is the union of the two operands' holidays, whose start date is the earlier of the two start dates, and whose length (in years) reaches at least the later of the two calendars' end years.

#### Scenario: Merged start date is the earlier of the two
- **WHEN** calendar A starts in 2023 and calendar B starts in 2020 and they are merged (in either order)
- **THEN** the merged calendar's start date is the 2020 date

#### Scenario: Merged length covers the later end
- **WHEN** calendar A spans 2020 for 5 years (through 2025) and calendar B spans 2023 for 3 years (through 2026) are merged
- **THEN** the merged calendar's start year is 2020 and its length reaches at least 2026

#### Scenario: Merged holidays are the union
- **WHEN** two calendars are merged
- **THEN** every holiday of either operand is present in the merged calendar, with duplicates collapsed

### Requirement: Rule-dispatched business-day adjustment
Business-day adjustment SHALL be an operation of the calendar, not of a date. `QCBusinessCalendar` SHALL expose `businessDay(date, rule)` returning the date adjusted according to a `QCBusDayAdjRules` value: unchanged for `qcNo`, `nextBusinessDay` for `qcFollow`, `modNextBusinessDay` for `qcModFollow`, and `previousBusinessDay` for both `qcPrev` and `qcModPrev`. The results SHALL be identical to those previously produced by `QCDate::businessDay` for the same date, holiday set, and rule. The equivalent `QCDate` overloads SHALL NOT be part of the C++ surface; in Python, `QCDate.business_day(holiday_list, rule)` SHALL continue to work and agree with `BusinessCalendar.business_day(date, rule)`.

#### Scenario: Following rule skips a holiday
- **WHEN** `businessDay` is called with `qcFollow` on a date that is a holiday
- **THEN** it returns the same date `nextBusinessDay` would return

#### Scenario: Modified following stays within the month
- **WHEN** `businessDay` is called with `qcModFollow` on a date whose next business day falls in the following month
- **THEN** it returns the preceding business day instead

#### Scenario: No adjustment returns the input
- **WHEN** `businessDay` is called with `qcNo`
- **THEN** it returns the input date unchanged, whether or not that date is a holiday or a weekend

#### Scenario: Modified previous behaves as previous
- **WHEN** `businessDay` is called with `qcModPrev`
- **THEN** it returns the same date as `qcPrev`, preserving the existing unimplemented-`qcModPrev` behavior

### Requirement: Adjustment does not reconstruct the holiday set
On the leg-building path, adjustment SHALL operate on the calendar's existing holiday set: no adjustment SHALL construct a `QCBusinessCalendar` or insert holidays as part of answering a query. Schedule generation SHALL carry a `QCBusinessCalendar` rather than a flattened holiday vector, so that no layer re-derives calendar structure a caller already holds.

Two compatibility surfaces are exempt and SHALL remain so only while their callers require holiday vectors: the `QCDate.business_day` Python binding, and the `asCalendar` helper in `QCFactoryFunctions.cpp`. Neither is reachable from leg building.

#### Scenario: Adjustment cost does not scale with the rebuild
- **WHEN** a leg is built against calendars holding 0, 120, and 420 holidays, with the number of in-range holidays and therefore the schedule work held constant
- **THEN** per-leg build time stays sub-millisecond in all three cases and rises by a small factor with holiday count (measured 0.036 / 0.107 / 0.242 ms), rather than by the two orders of magnitude the per-adjustment rebuild caused (43.6 ms at 420 holidays)
- **AND** the residual growth is attributable to the single holiday-set copy `QCInterestRatePeriodsFactory` makes when storing its calendar by value, not to repeated reconstruction

#### Scenario: Schedule generation holds a calendar
- **WHEN** `QCInterestRatePeriodsFactory` is constructed for a leg
- **THEN** it receives and stores a `QCBusinessCalendar`, and no caller flattens a calendar to a holiday vector on its behalf

