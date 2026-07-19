# business-calendar

## ADDED Requirements

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
Business-day adjustment SHALL operate on the calendar's existing holiday set. No adjustment operation SHALL construct a new `QCBusinessCalendar`, copy the holiday set, or insert holidays as part of answering a query. Schedule generation SHALL carry a `QCBusinessCalendar` rather than a flattened holiday vector, so that no layer re-derives calendar structure that a caller already holds.

#### Scenario: Adjustment cost is independent of holiday count
- **WHEN** a leg is built against calendars holding 0, 120, and 420 holidays, with the number of in-range holidays and therefore the schedule work held constant
- **THEN** the per-leg build time is of the same order in all three cases, rather than scaling with the size of the holiday set

#### Scenario: Schedule generation holds a calendar
- **WHEN** `QCInterestRatePeriodsFactory` is constructed for a leg
- **THEN** it receives and stores a `QCBusinessCalendar`, and no caller flattens a calendar to a holiday vector on its behalf
