# Design: fix-compounded-overnight-eq-rate-state

## Context

`CompoundedOvernightRateCashflow2` represents a compounded overnight (e.g. SOFR) coupon.
Its interest is `notional · (wf(startDate, endDate; r·gearing + spread) − 1)`, where the
compounded equivalent rate `r` is rounded to `_eqRateDecimalPlaces` per market convention.

`_endDateWf` and `_initialDateWf` are the object's cached compounded wealth factors. The
canonical relation the class relies on is:

```
coupon equivalent rate  r  =  getRateFromWf(_endDateWf / _initialDateWf, startDate, endDate)
```

`getRateFromWf` (`QCInterestRate.cpp:67`) is a pure, unrounded inversion `(_endDateWf − 1)/yf`
(linear/Act360). For the relation to yield the coupon rate, `_endDateWf` must be the wealth
factor **of that rate**.

Two code paths populate `_endDateWf`, and today they disagree on what it means:

| Populating path | `_endDateWf` set to | Inverts to |
|---|---|---|
| `ForwardRates` realized branch (`ForwardRates.h:168-172`) | `wf(round(accruedFixing))` | rounded rate ✓ |
| `fixing()` / `setFixings()` (`CompoundedOvernightRateCashflow2.cpp:143`) | `producto` (raw ∏ daily wf) | **unrounded** rate ✗ |

`amount()` (`:57`), which the whole system uses for PV, `record()["cashflow"]`, and `wrap()`,
inverts `_endDateWf` assuming the first meaning. When populated by `fixing()`, it recovers the
unrounded rate — while `interest()`/`settlementAmount()` use the rounded return of `fixing()`.
Hence PV/settlement disagree by the rounding for realized coupons.

Constraints:
- **`amount()` must not round** — valuation must stay differentiable (`_amountDerivatives`,
  DV01). This is a hard requirement; the fix must not introduce a `round()` into any path
  `amount()` or the sensitivity code traverses.
- The Python API surface (`qcf_binder.cpp`) must not change.
- `CompoundedOvernightRateMultiCurrencyCashflow2` inherits this machinery; the fix must land
  where both classes route through (they share `fixing()` / `_endDateWf`).

## Goals / Non-Goals

**Goals:**
- Restore the invariant "`_endDateWf` encodes the coupon's equivalent rate" on the
  `setFixings()`/`fixing()` path, so `amount()` and `settlementAmount()` agree for a realized
  coupon **without `amount()` rounding**.
- A minimal, single-method, trivially reversible change.
- A regression test that fails on `master` and passes after the fix.

**Non-Goals:**
- Changing `amount()` — it stays an unrounded inversion (differentiability). Untouched.
- Separating valuation interest from the settlement `_interest` member (the "amount() writes
  `_interest`" side effect). Once `amount()` and `settlementAmount()` converge for realized
  coupons, the clobber writes back the same value, so this becomes moot; deferred.
- Rounding the daily fixings inside `_getFixingWf` (the `// Aquí hacer un round a 4 decimales`
  TODO at `:222`). Separate concern; `setFixings()` already pre-rounds `_fixedRates` to 4
  places before calling `fixing()`.
- Thread-safety of the shared `_index` (`setRateValue` mutation). Separate concern.
- Touching the deprecated `CompoundedOvernightRateCashflow` (no `2`).

## Decisions

**D1 — Fix `fixing()` to store `_endDateWf = wf(rounded eqRate)` instead of the raw product.**

Current (`CompoundedOvernightRateCashflow2.cpp:139-147`):

```cpp
double producto = 1.0;
for (auto& fix_date: _fixingDates) {
    producto *= _getFixingWf(fix_date, fixings);
}
_endDateWf = producto;                                                   // raw product
auto eqRate = _index->getRate().getRateFromWf(producto, _startDate, _endDate);
double factor = std::pow(10, _eqRateDecimalPlaces);
return std::round(eqRate * factor) / factor;
```

Fixed:

```cpp
double producto = 1.0;
for (auto& fix_date: _fixingDates) {
    producto *= _getFixingWf(fix_date, fixings);
}
auto eqRate = _index->getRate().getRateFromWf(producto, _startDate, _endDate);
double factor = std::pow(10, _eqRateDecimalPlaces);
auto roundedEqRate = std::round(eqRate * factor) / factor;
// Store the wealth factor of the ROUNDED, BARE equivalent rate (no gearing/spread —
// amount()/_calculateInterest apply those when they re-invert). This keeps the invariant
// that amount()'s unrounded inversion of _endDateWf recovers the coupon rate, matching the
// ForwardRates realized branch (ForwardRates.h:168-172).
_index->setRateValue(roundedEqRate);
_endDateWf = _index->getRate().wf(_startDate, _endDate);
return roundedEqRate;
```

**BARE rate is essential.** `amount()` applies gearing/spread *after* inverting
(`_calculateInterest` does `setRateValue(rate·gearing + spread)`). Storing `wf(bare
roundedEqRate)` lets `amount()` recover `roundedEqRate` and then apply gearing/spread once. If
we stored `wf(roundedEqRate·gearing + spread)`, `amount()` would double-apply them.

**Roundtrip is exact.** After `setRateValue(roundedEqRate)`, `_endDateWf =
wf(roundedEqRate)`; `amount()` computes `getRateFromWf(_endDateWf)` over the same
`(_startDate, _endDate)` with the same year-fraction and wealth-factor objects — the exact
inverse — recovering `roundedEqRate` (linear Act360: `(1 + r·yf − 1)/yf = r`).

**Return value unchanged.** `fixing()` still returns `roundedEqRate`, so
`interest()`/`settlementAmount()` are numerically unchanged.

**D2 — `amount()`, `_calculateInterest()`, `_getRateValue()` are NOT edited.** They already do
the right thing *given a correct `_endDateWf`*. Not rounding in `amount()` is the point: the
rounding is baked into `_endDateWf` upstream, in the realized-only `fixing()` path, so the
differentiable forward path never sees it.

**D3 — Differentiability is provably untouched.** The forward/projected regime is populated by
`ForwardRates::setRateCompoundedOvernightCashflow2` else-branches (`ForwardRates.h:203-226`),
which set `_endDateWf` from curve discount factors and populate `_amountDerivatives`. That code
**never calls `fixing()`**. `setFixings()`/`fixing()` handle only fully realized coupons
(`_getFixingWf` throws if any fixing is missing — `:229`), whose `_amountDerivatives` stay at
the constructor default `[0.0, 0.0]` (zero curve sensitivity, correct for a realized coupon).
So D1 touches only the zero-sensitivity path and cannot perturb any DV01.

## Risks / Trade-offs

- **[Numbers change for realized coupons priced via `setFixings()` + `amount()`/`record()`/
  `wrap()`/PV]** → Bug fix; they now match `settlementAmount()`. Call out in the commit
  message. The two disagreed before, so no correct result is lost. Coupons priced through
  `ForwardRates` (the normal valuation entry point) are unchanged.
- **[`_getRateValue()` / `record()["rate_value"]` now reports the rounded eq-rate for realized
  coupons]** → This is the contractual rate; arguably more correct. It was unrounded before.
  Note in the test and commit message.
- **[Roundtrip depends on `wf`/`getRateFromWf` being mutual inverses]** → They are, for any
  monotone wealth factor over the same dates; the overnight eq-rate is Lin Act360. Low risk.
- **[Non-linear/compound index rate]** → Not applicable here (Lin Act360), and the roundtrip
  still holds because both directions use the same `_wf`/`_yf` on the same dates.

## Migration Plan

Single library change; no data or API migration. Version bump `1.12.0a3 → 1.12.0a4` in
`setup.py`, `CLAUDE.md`, and `source/qcf_binder.cpp` `id()`. Commit on `develop` with message
`# Update to Version 1.12.0a4: …`.

**Rollback = revert the `fixing()` body** (restore `_endDateWf = producto` and drop the
`setRateValue`/`wf` lines) plus the version strings. One method; no other file depends on the
new behavior.

## Open Questions

- None blocking. If the Catch2 test target stays commented out in `Tests/CMakeLists.txt`, the
  regression test is exercised via the Python bindings (see tasks).
