## 1. Fix _buildBasicDates2 — SHORTFRONT branch

- [x] 1.1 Locate the `qcShortFront` block in `source/QCInterestRatePeriodsFactory.cpp` (approx. lines 700–750)
- [x] 1.2 Replace the `moveToDayOfMonth` forward-walk with `_endDate.addMonths(-(numWholePeriods * (int)tenor))` to compute the raw stub end
- [x] 1.3 Apply `businessDay(calendar, _endDateAdjustment)` to the result and store it as `pseudoStartDate` before the period-building loop
- [x] 1.4 Remove the post-loop fix-up lines that patch `periods[0].end` and `periods[1].start` (they are no longer needed)

## 2. Fix _buildBasicDates2 — LONGFRONT branch

- [x] 2.1 Locate the `qcLongFront` block (approx. lines 752–790)
- [x] 2.2 Apply the same backward-anchor replacement as in task 1.2–1.4

## 3. Fix _buildBasicDates2 — LONGFRONT2 branch

- [x] 3.1 Locate the `qcLongFront2` block (approx. lines 792–844)
- [x] 3.2 Apply the same backward-anchor replacement as in task 1.2–1.4

## 4. Fix _auxWithLongFrontNNotEasyCase

- [x] 4.1 Locate `_auxWithLongFrontNNotEasyCase` in `source/QCInterestRatePeriodsFactory.cpp` (approx. lines 955–1009)
- [x] 4.2 Apply the same backward-anchor replacement: `pseudoStartDate = _endDate.addMonths(-(numWholePeriods * (int)tenor)).businessDay(calendar, _endDateAdjustment)`
- [x] 4.3 Remove the post-loop fix-up for `periods[0].end` / `periods[1].start` if present

## 5. Add regression tests

- [x] 5.1 Add a `TEST_CASE` in `Tests/LegFactoryTests.cpp` covering `startDate=2025-01-31, endDate=2025-09-30, 3M, FOLLOW, SHORTFRONT` — assert exactly 3 periods with end dates 2025-03-31, 2025-06-30, 2025-09-30
- [x] 5.2 Add a test for LONGFRONT non-easy case with a day overflow (stub base day > endDate.day())
- [x] 5.3 Add a test for LONGFRONT2 non-easy case with a day overflow
- [x] 5.4 Add a test for LONGFRONTn (via `_auxWithLongFrontNNotEasyCase`) with a day overflow

## 6. Build and smoke test

- [x] 6.1 Build the Python wheel: `python setup.py bdist_wheel`
- [x] 6.2 Install the wheel into `.venv` and run the confirmed smoke test (`startDate=2025-01-31, endDate=2025-09-30, 3M, FOLLOW, SHORTFRONT`) — verify `Bug present: False`
- [x] 6.3 Run any existing test suite to confirm no regressions

## 7. Version bump and commit

- [x] 7.1 Bump `version=` in `setup.py` from `1.10.1` to `1.10.2`
- [ ] 7.2 Commit with message: `# Update to Version 1.10.2: fix front-stub pseudoStartDate overshoot in _buildBasicDates2`
