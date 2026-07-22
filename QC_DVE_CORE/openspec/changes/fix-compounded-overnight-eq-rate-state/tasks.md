# Tasks: fix-compounded-overnight-eq-rate-state

## 1. Fix the state invariant

- [x] 1.1 In `source/cashflows/CompoundedOvernightRateCashflow2.cpp` `fixing()`: after
  computing `roundedEqRate`, set `_endDateWf` to the wealth factor of the **bare rounded**
  eq-rate — `_index->setRateValue(roundedEqRate); _endDateWf = _index->getRate().wf(_startDate, _endDate);`
  — and delete the old `_endDateWf = producto;` line. Return value stays `roundedEqRate`
  (design D1). Do **not** apply gearing/spread here — `amount()`/`_calculateInterest` apply
  them on re-inversion.
- [x] 1.2 Confirm no other method assigns `_endDateWf` from a raw product (grep `_endDateWf`):
  only `fixing()`, `setEndDateWf()` (used by `ForwardRates`), and the constructor default.
  `amount()`, `_getRateValue()`, `_calculateInterest()` are left unedited (design D2).

## 2. Regression tests

- [x] 2.1 Realized-coupon consistency: build a `CompoundedOvernightRateCashflow2` with a full
  set of fixings, `spread != 0`, `gearing != 1`, and a small `_eqRateDecimalPlaces` (e.g. 6 or
  8) so the rounding gap is nonzero; call `setFixings()`. Assert `amount()` (minus
  `amortization` when `doesAmortize`) equals `settlementAmount()` minus `amortization` and
  equals `interest(fixings)`, to double precision. On `master` this fails; after the fix it
  passes.
- [x] 2.2 No-clobber: after `setFixings()`, call `amount()`, then assert `interest(fixings)`
  still returns the rounded interest (i.e. `amount()` no longer leaves a divergent `_interest`).
- [x] 2.3 `rate_value` reports the rounded eq-rate: after `setFixings()`,
  `record()["rate_value"]` equals `round(eqRate, _eqRateDecimalPlaces)` (the value returned by
  `fixing()`), and `record()["cashflow"]` equals `record()["interest"] (+ amortization)`.
- [x] 2.4 Forward-path unchanged: a coupon priced through
  `ForwardRates::setRatesCompoundedOvernightLeg2` (not `setFixings()`) produces the same
  `amount()`, PV, and `_amountDerivatives` as on `master` (byte-for-byte). Guards D3.
  **Covered structurally, not by a new test** (documented in the test file header): `amount()`
  and the `ForwardRates` code are unchanged, and task 1.2 established `fixing()` is the only
  non-`ForwardRates` writer of `_endDateWf` and is realized-only. The forward setters
  (`set_initial_date_wf`/`set_end_date_wf`) are not bound to Python, so a Python-only
  byte-for-byte test is not possible without standing up a `ZeroCouponCurve`; deferred as
  low-value given the structural guarantee.

## 3. Release

- [x] 3.1 Bump version `1.12.0a3 → 1.12.0a4` in `setup.py:149`, `CLAUDE.md:9`, and the `id()`
  string at `source/qcf_binder.cpp:106`.
- [x] 3.2 Build the wheel for the current pyenv version and smoke-test a realized
  `CompoundedOvernightRateCashflow2` from Python (`amount`, `settlement_amount`, `record`) —
  confirm `amount` matches `settlement_amount` and matches the manually computed
  `notional · (round(eqRate)·gearing + spread) · yf`.
- [ ] 3.3 Commit on `develop`: `# Update to Version 1.12.0a4: fix CompoundedOvernightRateCashflow2 realized-coupon eq-rate state (fixing() stores wf of rounded eq-rate so amount() matches settlementAmount without rounding)`.

## 4. Reversal checklist (if the fix does not resolve the observed issue)

- [ ] 4.1 Revert the `fixing()` body to `_endDateWf = producto;` (drop the `setRateValue`/`wf`
  lines); revert the three version strings. Confirm no other file references the new behavior
  (task 1.2 already establishes this).
