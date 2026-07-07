# Tasks: fix-overnight-index-eq-rate-state

## 1. Consolidate eq-rate computation

- [x] 1.1 Add private helper `_eqRateAt(QCDate& date, double indexValue, unsigned int decimalPlaces)` in `OvernightIndexCashflow.h/.cpp` holding all guards (date <= start → 0, yf == 0 → 0, > 12 places → no rounding); make the existing public `getEqRate(date, indexValue)` forward to it with `_eqRateDecimalPlaces`. Distinct name, not an overload — an overload breaks the `&OvernightIndexCashflow::getEqRate` member pointer at `qcf_binder.cpp:917` (see design D1)
- [x] 1.2 Add private helper `_endDateForEqRate()` returning `_endDate` (qcAccrual) or `_indexEndDate` (qcIndex); use it in `getRateValue()`, `wrap()`, `record()` to remove the duplicated if/else blocks
- [x] 1.3 Rewrite `settlementAmount()` to call the consolidated overload (delete the inline `round(((endIdx/startIdx - 1)/yf) * factor)` block)

## 2. Fix the state bugs

- [x] 2.1 Fix `amount()`: apply gearing (`eqRate * _gearing + _spread`), pass unrounded intent via the decimalPlaces parameter, delete the `_eqRateDecimalPlaces` save/mutate/restore
- [x] 2.2 Fix `record()` to compute interest with the eq-rate evaluated at `_endDateForEqRate()` (wf still over accrual period), so `interest` and `rate_value` agree in qcIndex mode
- [x] 2.3 Fix `OvernightIndexMultiCurrencyCashflow::settlementCurrencyInterest()` and `mccyWrap()` the same way (route through the shared helper so both classes use one code path)
- [x] 2.4 1-arg `accruedInterest(date, indexValue)` and TimeSeries overload: when `date >= _endDate`, evaluate the eq-rate at `_endDateForEqRate()` (TimeSeries overload looks up the fixing at the eq-rate date); mid-period behavior unchanged. Update the three `accrued_interest` docstrings in `qcf_binder.cpp` (design D7)

## 3. Regression tests

- [x] 3.1 Test: `amount()` with `gearing = 2.0` matches `settlementAmount()` minus amortization (up to rounding); today this fails
- [x] 3.2 Test: `record()` in qcIndex mode with index dates shifted from accrual dates — `interest` consistent with `rate_value`; today this fails
- [x] 3.3 Test: zero-length period through `settlementAmount()` returns amortization (no NaN); `getEqRateDecimalPlaces()` unchanged after `amount()`
- [x] 3.4 Consistency-matrix test, qcAccrual mode: 1-arg `accruedInterest(endDate, endIdx)`, 3-arg `accruedInterest(endDate, endDate, endIdx)`, `record()["interest"]`, `wrap()` interest, and `settlementAmount() - amortization` all agree (exact for the first three, up to currency rounding for the last two); use nonzero spread and gearing
- [x] 3.5 Consistency-matrix test, qcIndex mode with index dates shifted from accrual dates: 1-arg `accruedInterest(endDate, endIdx)`, 3-arg `accruedInterest(endDate, indexEndDate, endIdx)`, TimeSeries overload (fixing at indexEndDate), `record()["interest"]`, and `settlementAmount() - amortization` all agree (same tolerances) — today the record() and 1-arg legs of this fail. Also: mid-period 1-arg call at date d matches 3-arg `(d, d, value)` (single-date contract preserved)
- [x] 3.6 Run existing OvernightIndex tests (Catch2 or Python-side) to confirm no change for gearing = 1 / qcAccrual cases

## 4. Release

- [x] 4.1 Bump version in `setup.py` (1.11.3 → 1.11.4)
- [x] 4.2 Build wheel for current pyenv version and smoke-test an `OvernightIndexCashflow` from Python (`amount`, `settlement_amount`, `record`)
- [x] 4.3 Commit on `develop` with message `# Update to Version 1.11.4: fix OvernightIndexCashflow eq-rate state bugs (gearing in amount, qcIndex dates in record, decimal-places mutation, settlementAmount guards)`
