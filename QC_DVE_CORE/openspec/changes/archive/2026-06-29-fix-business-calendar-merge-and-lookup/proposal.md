## Why

A review of `source/time/QCBusinessCalendar.cpp` surfaced two defects:

1. **`operator+` computes the wrong merged start date.** The branch meant to pick the earlier of the two start dates assigns `_startDate` to itself (`min_date = _startDate;`) instead of `cal.getStartDate()`, so it is a no-op. When the right-hand calendar starts earlier, the merged calendar reports the later start date. This is exposed to Python as `BusinessCalendar.__add__`.
2. **Business-day lookups scan the holiday set linearly.** `nextBusinessDay` / `previousBusinessDay` test holiday membership with `std::binary_search(_holidays.begin(), _holidays.end(), …)`. The result is correct (the `std::set` is ordered by `QCDate::operator<`, which compares `excelSerial()`), but because `std::set` iterators are bidirectional, `binary_search` degrades to O(n) per lookup — defeating the set's O(log n) lookup. These functions are on hot paths (schedule generation, present-value loops), and merged multi-market calendars hold many holidays.

## What Changes

- Fix `operator+` so the merged calendar's start date is the **earlier** of the two start dates, and its length spans **at least through the later** of the two calendars' end years (so the metadata covers the union). The holiday set remains the union of both (already correct).
- Replace `std::binary_search(_holidays.begin(), _holidays.end(), d)` with the set's own logarithmic lookup (`_holidays.find(d) != _holidays.end()` / `_holidays.count(d)`) in `nextBusinessDay` and `previousBusinessDay`. Behavior is unchanged; complexity drops from O(n) to O(log n).
- Add tests for the merge metadata and for business-day adjustment over holidays/weekends.

No public API changes. `nextBusinessDay` / `previousBusinessDay` / `shift` keep identical results; only `operator+`'s reported start/length change (toward correctness).

## Capabilities

### New Capabilities
- `business-calendar`: the holiday-aware business-day adjustment (`nextBusinessDay`, `previousBusinessDay`, `shift`, `modNextBusinessDay`) and calendar merge (`operator+`) behavior of `QCBusinessCalendar`, including the logarithmic holiday-membership requirement and the merge span rules.

### Modified Capabilities
<!-- None — QCBusinessCalendar has no existing spec. -->

## Impact

- **Modified code:** `source/time/QCBusinessCalendar.cpp` (`operator+`, `nextBusinessDay`, `previousBusinessDay`). No header change.
- **Behavior:** `operator+` now reports the correct earlier start date and a covering length; Python `BusinessCalendar.__add__` is corrected. Business-day adjustment results are unchanged (only faster).
- **Performance:** holiday membership becomes O(log n) per step on the hot paths.
- **Tests:** `Tests/QCBusinessCalendarTests.cpp` (merge metadata + adjustment over holidays/weekends).
- **Not affected:** `CalendarFactory`, which populates a single calendar via `addHoliday` and does not use `operator+`.
