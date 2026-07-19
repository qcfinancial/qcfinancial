# business-calendar

## ADDED Requirements

### Requirement: Rule-dispatched business-day adjustment
`QCBusinessCalendar` SHALL expose `businessDay(date, rule)` returning the date adjusted according to a `QCBusDayAdjRules` value: unchanged for `qcNo`, `nextBusinessDay` for `qcFollow`, `modNextBusinessDay` for `qcModFollow`, and `previousBusinessDay` for both `qcPrev` and `qcModPrev`. The results SHALL be identical to those previously produced by `QCDate::businessDay` for the same date, holiday set, and rule.

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

## REMOVED Requirements

### Requirement: Business-day adjustment as a date operation
**Reason**: Adjustment requires a holiday set, so it belongs to the type that owns one. `QCDate`'s calendar-taking overloads rebuilt a `QCBusinessCalendar` on every call to answer a single query, which was the dominant cost of leg construction. `QCBusinessCalendar` already implemented every branch of the adjustment; only the rule dispatch was on `QCDate`, and it has moved.

**Migration**: In C++, replace `date.businessDay(holidays, rule)` with `calendar.businessDay(date, rule)`, and `date.shift(holidays, n, …)` with the pre-existing `calendar.shift(date, n)`. `QCDate::addWeeks` had no live caller and no binding, and is removed without replacement; equivalent behavior is `calendar.businessDay(date.addDays(7 * n), rule)`. In Python, `QCDate.business_day(holiday_list, rule)` continues to work unchanged; `BusinessCalendar.business_day(date, rule)` is the preferred surface.
