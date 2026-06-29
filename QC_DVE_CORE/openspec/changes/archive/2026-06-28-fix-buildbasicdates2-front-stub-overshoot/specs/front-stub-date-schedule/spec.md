## ADDED Requirements

### Requirement: Front-stub schedule anchored from end date
When a leg has a front stub period (SHORTFRONT, LONGFRONT, or LONGFRONTn) and the date range is not an exact multiple of the settlement periodicity, the stub period end date SHALL be computed as `endDate - (numWholePeriods × tenor)`, not by advancing from `startDate + remainderInMonths` via `moveToDayOfMonth`. This guarantees that `pseudoStartDate + numWholePeriods × tenor == endDate` (before business-day adjustment) — the only anchor that makes the last regular period end exactly at `endDate`.

#### Scenario: SHORTFRONT — end day earlier than stub base day
- **WHEN** `startDate = 2025-01-31`, `endDate = 2025-09-30`, periodicity = 3M, stub = SHORTFRONT, endDateAdjustment = FOLLOW
- **THEN** the schedule SHALL contain exactly 3 periods with end dates 2025-03-31 (stub), 2025-06-30, 2025-09-30

#### Scenario: SHORTFRONT — numPeriods == 1 (stub only)
- **WHEN** the date range produces a single stub period with no whole periods
- **THEN** the schedule SHALL contain exactly 1 period from `startDate` to `endDate` (after business-day adjustment)

#### Scenario: LONGFRONT — end day earlier than stub base day
- **WHEN** `startDate` and `endDate` yield `remainderInMonths != 0` with a LONGFRONT stub and `startDate.addMonths(remainderInMonths + tenor).day() > endDate.day()`
- **THEN** the schedule SHALL produce whole periods whose last end date equals `endDate` (after business-day adjustment), with no period overshooting `endDate`

#### Scenario: LONGFRONTn — end day earlier than stub base day
- **WHEN** a LONGFRONTn (n = 2..14) stub is used and the stub base day exceeds `endDate.day()`
- **THEN** the schedule SHALL produce whole periods whose last end date equals `endDate` (after business-day adjustment)

### Requirement: Business-day adjustment applied before anchor use
The business-day adjustment for `pseudoStartDate` SHALL be applied before `pseudoStartDate` is used as the anchor for computing regular period end dates. Anchoring from an unadjusted date causes 1–2 day errors in all subsequent period end dates when the stub end falls on a non-business day.

#### Scenario: Stub end on a weekend
- **WHEN** `_endDate.addMonths(-(numWholePeriods * tenor))` lands on a Saturday or Sunday
- **THEN** the adjusted `pseudoStartDate` SHALL be the next business day (FOLLOW) or the preceding business day (PRECEDING) per `endDateAdjustment`, and all subsequent regular period end dates SHALL be anchored from this adjusted date

#### Scenario: Stub end on a business day
- **WHEN** `_endDate.addMonths(-(numWholePeriods * tenor))` lands on a business day
- **THEN** `pseudoStartDate` is unchanged by business-day adjustment and regular period end dates are identical to the unadjusted-anchor case
