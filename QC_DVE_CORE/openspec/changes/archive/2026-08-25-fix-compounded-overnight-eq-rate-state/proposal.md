# Proposal: fix-compounded-overnight-eq-rate-state

## Why

For a **fully realized** `CompoundedOvernightRateCashflow2` (one whose fixings are all
known, populated via `setFixings()`), `amount()` and `settlementAmount()` disagree on
the coupon interest, and the disagreement equals the equivalent-rate rounding.

Root cause is a single state-representation mismatch. `amount()` recovers the coupon
rate by inverting the stored wealth factor:

```
amount()  (CompoundedOvernightRateCashflow2.cpp:57)
    rate = getRateFromWf(_endDateWf / _initialDateWf, _startDate, _endDate)   // no rounding
    _interest = _calculateInterest(rate, _endDate)
```

This is correct *by design* — valuation must stay differentiable, so `amount()` must not
round. It relies on the invariant that **`_endDateWf` encodes the coupon's equivalent
rate**. `ForwardRates` upholds that invariant for realized coupons by reconstructing
`_endDateWf` from the already-rounded `accruedFixing` (`ForwardRates.h:168-172`).

But the direct `setFixings()` path breaks it. `fixing()` stores the **raw daily-compounded
product** instead of the wealth factor of the rounded equivalent rate:

```
fixing()  (CompoundedOvernightRateCashflow2.cpp:139-146)
    producto = ∏ daily wf                      // raw product
    _endDateWf = producto                       // ← raw product, NOT wf(rounded eqRate)
    return round((producto-1)/yf, _eqRateDecimalPlaces)
```

So `amount()` inverts `producto` back to the **unrounded** rate, while
`interest()`/`settlementAmount()` use the **rounded** return value of `fixing()`. The
equivalent rate is correct; the interest amount from `amount()` is off by
`notional · gearing · (eqRate − round(eqRate, _eqRateDecimalPlaces)) · yf`.

Two secondary effects follow from the same mismatch:
- `amount()` overwrites `_interest` (`:58`), so calling `amount()`/`record()`/`wrap()` after
  `setFixings()` silently replaces the correct rounded interest with the unrounded one; the
  reported settlement interest then depends on call order.
- `_getRateValue()` (`:360`, used by `record()["rate_value"]` and `wrap()`) inverts the same
  raw `_endDateWf` and reports the **unrounded** equivalent rate.

## What Changes

- In `CompoundedOvernightRateCashflow2::fixing()`, store `_endDateWf` as the wealth factor
  of the **rounded** equivalent rate (bare rate — no gearing/spread), instead of the raw
  compounded product. This restores the invariant `amount()` depends on and mirrors exactly
  what `ForwardRates.h:168-172` already does for realized coupons. The **return value of
  `fixing()` is unchanged** (still the rounded eq-rate).
- No change to `amount()`. It stays an unrounded inversion of `_endDateWf`, preserving
  differentiability. For a realized coupon it now recovers the rounded rate because
  `_endDateWf` encodes it; for a projected coupon (populated by `ForwardRates`, which never
  calls `fixing()`) behavior is byte-for-byte unchanged.
- Regression tests: for a realized coupon populated via `setFixings()`, assert
  `amount()` equals `settlementAmount()` (before amortization) and equals
  `interest(fixings)`, with a nonzero spread and gearing ≠ 1 so the rounding gap is
  exercised.
- Version bump in `setup.py`, `CLAUDE.md`, and the binder `id()` string per project
  convention.

Numbers change only for **fully realized `CompoundedOvernightRateCashflow2` coupons
populated via `setFixings()`**: `amount()`, `record()["cashflow"]`, `record()["rate_value"]`,
`wrap()` interest/rate, and their PV now report the rounded contractual figure, matching
`settlementAmount()`. This is a bug fix (the two disagreed before, so no correct downstream
number is lost). Projected/forward coupons and multi-currency coupons priced through
`ForwardRates` are unaffected.

## Capabilities

### New Capabilities

- `compounded-overnight-rate-cashflow`: Equivalent-rate state invariant for
  `CompoundedOvernightRateCashflow2` — `_endDateWf` encodes the coupon's equivalent rate, so
  the unrounded valuation inversion in `amount()` and the rounded settlement path agree for a
  realized coupon, without `amount()` itself rounding.

### Modified Capabilities

<!-- none — no existing spec covers CompoundedOvernightRateCashflow2 -->

## Impact

- `source/cashflows/CompoundedOvernightRateCashflow2.cpp` — `fixing()` only (≈4-line body
  change). No header change. No signature change.
- `Tests/` — new/updated Catch2 (or Python-side) consistency test for a realized coupon.
- No Python binding signature changes; `qcfinancial` users see corrected numbers only in the
  realized-`setFixings()` case above.
- `setup.py`, `CLAUDE.md`, `source/qcf_binder.cpp` version strings.
- **Rollback**: revert the `fixing()` body (restore `_endDateWf = producto`). Single method,
  no schema/API/data migration.
