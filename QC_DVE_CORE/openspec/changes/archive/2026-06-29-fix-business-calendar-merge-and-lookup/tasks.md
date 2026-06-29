## 1. Fix operator+

- [x] 1.1 In `QCBusinessCalendar::operator+` (`source/time/QCBusinessCalendar.cpp`), set the merged start date to the earlier of `_startDate` and `cal.getStartDate()` (fix the no-op `min_date = _startDate;` branch)
- [x] 1.2 Set the merged length to span at least the later end year: `max(_startDate.year()+_length, cal.getStartDate().year()+cal.getLength()) - mergedStart.year()`
- [x] 1.3 Keep the holiday union as-is (already correct)

## 2. Logarithmic holiday lookup

- [x] 2.1 Replace `std::binary_search(_holidays.begin(), _holidays.end(), fechaOut)` with `_holidays.find(fechaOut) != _holidays.end()` (or `_holidays.count(...)`) in `nextBusinessDay`
- [x] 2.2 Same replacement in `previousBusinessDay`
- [x] 2.3 Confirm `<algorithm>` is no longer required for this (leave the include if other uses remain)

## 3. Tests

- [x] 3.1 Add/extend `Tests/QCBusinessCalendarTests.cpp`: weekday holiday skipped, Friday holiday → Monday, Monday holiday → previous Friday, run of consecutive holidays across a weekend
- [x] 3.2 Add merge tests: earlier right-hand start wins; length covers the later end; holiday set is the union

## 4. Build and verify

- [x] 4.1 Build the wheel and smoke-test from Python: `cal_a + cal_b` reports the earlier start; `next_busy_day` / `prev_busy_day` over a known holiday/weekend match expectations
- [x] 4.2 Bump `version=` in `setup.py` and update the `id()` build string in `source/qcf_binder.cpp`

## 5. Commit

- [x] 5.1 Commit with message `# Update to Version X.Y.Z: fix QCBusinessCalendar operator+ start/length and use O(log n) holiday lookup`
