## Context

`QCInterestRatePeriodsFactory::_buildBasicDates2` generates the `(startDate, endDate)` pairs for each period in a leg. For front-stub cases (`SHORTFRONT`, `LONGFRONT`, `LONGFRONT2`–`LONGFRONT14`) when `easyCase = false` (i.e., the date range is not an exact multiple of the periodicity), it computes a `pseudoStartDate` — the end of the stub period — from which all regular period end dates are anchored.

**Current (broken) logic for `SHORTFRONT` when `numPeriods > 1`:**
```
pseudoStartDate = _startDate.addMonths(remainderInMonths)
                            .moveToDayOfMonth(_endDate.day(), qcForward)
```

`moveToDayOfMonth(day, qcForward)` walks forward one day at a time until `result.day() == day`. When the base date's day-of-month is already past the target day, it crosses into the next month. For example: base = March 31, target day = 30 → walks to April 30 (one month too late).

**Correct invariant:** `pseudoStartDate + numWholePeriods × tenor` must equal `_endDate` (before business-day adjustment). The only anchor that satisfies this invariant is `_endDate - numWholePeriods × tenor`.

This approach is what the deprecated `_buildBasicDates` already used (it built periods backward from `_endDate`), and it is the industry-standard method for front-stub schedule generation.

## Goals / Non-Goals

**Goals:**
- `pseudoStartDate` is computed as `_endDate.addMonths(-(numWholePeriods × tenor))` so the last regular period always ends exactly at `_endDate` (after business-day adjustment).
- Business-day adjustment is applied to `pseudoStartDate` **before** it is used as the anchor, eliminating the secondary 1–2 day drift.
- The fix covers all front-stub cases in `_buildBasicDates2` and in `_auxWithLongFrontNNotEasyCase`.

**Non-Goals:**
- Changing behavior for `SHORTBACK`, `LONGBACK`, or `NO` stub periods (unaffected).
- Changing behavior for the `easyCase` path (unaffected — it already anchors correctly from `_startDate`).
- Modifying the deprecated `_buildBasicDates` / `_getPeriods1` (already deprecated and unused).

## Decisions

### Decision 1: Anchor `pseudoStartDate` from `_endDate` going backward, not from `_startDate` going forward

**Chosen:** `pseudoStartDate = _endDate.addMonths(-(numWholePeriods * tenor)).businessDay(calendar, _endDateAdjustment)`

**Alternatives considered:**
- *Fix `moveToDayOfMonth` direction conditionally*: Check if `base.day() > target_day` and switch to `qcBackward`. Rejected — this still leaves `moveToDayOfMonth` as the anchor mechanism, which is fragile for end-of-month dates (e.g., Feb 28 → target day 31).
- *Add `stopAtEndOfMonth` to `SHORTFRONT`* (like `LONGFRONT` already does): Rejected — partially mitigates end-of-month cases but doesn't fix the general overshoot.
- *Keep forward anchor, clamp to same month*: Rejected — adds complexity and edge cases; backward-from-end is cleaner and matches the deprecated implementation.

### Decision 2: Apply `businessDay` to `pseudoStartDate` before using it as anchor

The post-loop fix-up (currently applied after the loop) corrects `periods[0].end` and `periods[1].start` but leaves `periods[1].end`, `periods[2].end`, etc. anchored from the unadjusted date. Moving the `businessDay` call to before the loop eliminates this secondary drift and makes the post-loop fixup unnecessary for the end-date anchor.

The post-loop lines updating `periods[0].end` and `periods[1].start` can be removed — the stub end is already adjusted before `pseudoStartDate` is set.

### Decision 3: Same fix applies to `_auxWithLongFrontNNotEasyCase`

The function is a factored-out helper for `LONGFRONT3`–`LONGFRONT14` non-easy cases. It has structurally identical code and the same bug. Fix it in the same pass.

## Risks / Trade-offs

- **Behaviour change for existing users with misconfigured legs**: Users who built legs with front stubs and wrong dates in production will now get different (correct) dates. This is intentional — it is a bug fix. → Communicate via release notes and version bump.
- **`LONGFRONT` partial fix**: The `LONGFRONT` branch in `_buildBasicDates2` already passes `_endDate.isEndOfMonth()` to `moveToDayOfMonth`, which provides partial protection. The same backward-anchor fix should be applied to `LONGFRONT` for full correctness. → Include in this fix.
- **No regression on `easyCase`**: The easy-case path (exact period alignment) is untouched. → Verify with existing tests.

## Migration Plan

1. Fix `_buildBasicDates2` and `_auxWithLongFrontNNotEasyCase` in `source/QCInterestRatePeriodsFactory.cpp`.
2. Add a regression test in `Tests/LegFactoryTests.cpp` covering the confirmed failure case (`startDate=2025-01-31`, `endDate=2025-09-30`, `3M`, `FOLLOW`, `SHORTFRONT`).
3. Add tests for `LONGFRONT` and `LONGFRONT2` non-easy cases with day overflow.
4. Build wheel for all supported Python versions and run the smoke test.
5. Bump version to `1.10.2` in `setup.py`.

## Open Questions

- Should `LONGBACK` and `SHORTBACK` be audited for a similar anchor issue in their last-period end date calculation? (The last period currently uses `fechaInicioPeriodo.addMonths(remainderInMonths).moveToDayOfMonth(...)` — potentially the same forward-walk issue.) → Out of scope for this fix but worth a follow-up.
