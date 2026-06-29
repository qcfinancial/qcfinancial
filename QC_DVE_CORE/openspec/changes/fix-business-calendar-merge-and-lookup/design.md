## Context

`QCBusinessCalendar` stores holidays in a `std::set<QCDate>` (ordered by `QCDate::operator<`, which compares `excelSerial()`) plus a Saturday/Sunday weekend definition. It offers business-day adjustment (`nextBusinessDay`, `previousBusinessDay`, `shift`, `modNextBusinessDay`) and a `+` operator to merge two calendars. The class has no OpenSpec coverage yet. A code review found a correctness bug in `operator+` and a complexity bug in the holiday lookups.

## Goals / Non-Goals

**Goals:**
- Correct `operator+` start-date and span metadata.
- Make holiday membership tests logarithmic.
- Preserve the exact results of all business-day adjustment functions.

**Non-Goals:**
- No change to the weekend model (Sat/Sun) or to `CalendarFactory`.
- No header/API signature changes.
- No change to pickling format (only the values `operator+` produces become correct).

## Decisions

### D1. `operator+`: earliest start, covering length, union of holidays
The merged calendar SHALL use `min(startA, startB)` as its start date and a length that reaches at least the later end year. Concretely, with `endYear(c) = c.start.year() + c.length`:

```
mergedStart  = min(startA, startB)
mergedLength = max(endYear(A), endYear(B)) - mergedStart.year()
mergedHols   = holidaysA ∪ holidaysB   (already correct)
```

*Why:* the current code's `min_date` branch is a no-op (`min_date = _startDate` in both paths), and `max_length` measured from the earliest start can fall short of the later end (e.g. A=2020/len5, B=2023/len3 → must reach 2026, not 2025). Computing the span from the union's true bounds keeps `getStartDate()`/`getLength()` honest. Holiday dates are unaffected (they are inserted regardless).

*Alternative considered:* leave length as `max(lenA, lenB)`. Rejected — it can under-report the covered span, which is the metadata callers and the pickle path rely on.

### D2. Holiday membership via the set's own lookup
Replace `std::binary_search(_holidays.begin(), _holidays.end(), d)` with `_holidays.find(d) != _holidays.end()` (or `_holidays.count(d) != 0`) in `nextBusinessDay` and `previousBusinessDay`.

*Why:* `std::set` iterators are bidirectional, so `binary_search` cannot random-access and degrades to a linear in-order walk — O(n) per lookup. The set's `find`/`count` is a O(log n) tree descent. Results are identical (same ordering, same equality). This is the single highest-leverage change since these functions run inside schedule generation and PV loops.

### D3. Preserve adjustment semantics exactly
The weekend-skip and consecutive-holiday loop logic is correct and stays as-is; only the membership predicate changes. Tests SHALL pin the existing results (holiday on a weekday, holiday on a Friday → next Monday, holiday on a Monday → previous Friday, runs of consecutive holidays) to guard against regression.

## Risks / Trade-offs

- **`operator+` output changes** → `getStartDate()`/`getLength()` (and pickled state) differ when the right-hand calendar starts earlier or extends later. This is a correctness fix, but any caller that depended on the buggy values would see a change. Mitigation: the holiday *set* is unchanged, so business-day results are identical; only metadata is corrected. Document in the change.
- **Equality/marshaling subtleties** → none expected; `find`/`count` use the same comparator that orders the set.

## Open Questions

None — both fixes are localized and the intended behavior is unambiguous.
