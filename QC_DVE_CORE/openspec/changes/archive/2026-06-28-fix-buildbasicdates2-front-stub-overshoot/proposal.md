## Why

`_buildBasicDates2` uses `moveToDayOfMonth(endDate.day(), qcForward)` to find the stub period end date for front-stub cases (`SHORTFRONT`, `LONGFRONT`, `LONGFRONT2`–`LONGFRONT14`). When `startDate.addMonths(remainderInMonths).day() > endDate.day()`, the forward walk crosses into the next month, making `pseudoStartDate` one full period too late. Every subsequent period end date is anchored from this wrong base, causing the leg to overshoot `endDate` by one full period — confirmed in production and by automated test.

## What Changes

- Fix the stub end date calculation in `_buildBasicDates2` for all front-stub cases so that `pseudoStartDate` is derived by going **backward** from `endDate` by `numWholePeriods × tenor`, not forward from `startDate + remainderInMonths`.
- Apply the same correction in `_auxWithLongFrontNNotEasyCase`, which has an identical structure and the same bug.
- Apply `businessDay` adjustment to `pseudoStartDate` **before** using it as the anchor for regular period end dates (fixes the secondary 1–2 day drift when the stub end falls on a non-business day).

## Capabilities

### New Capabilities
- `front-stub-date-schedule`: Correct schedule generation for legs with a front stub period when the date range is not an exact multiple of the settlement periodicity.

### Modified Capabilities

## Impact

- `source/QCInterestRatePeriodsFactory.cpp` — `_buildBasicDates2` (lines ~725–790 for `SHORTFRONT` and `LONGFRONT`) and `_auxWithLongFrontNNotEasyCase` (~955–1009).
- All four leg types that use `_buildBasicDates2` for settlement period generation are affected: `FixedRate`, `Ibor`, `OvernightIndex`, and `CompoundedOvernightRate` (and their multi-currency variants).
- No API or Python binding changes — purely internal date arithmetic.
