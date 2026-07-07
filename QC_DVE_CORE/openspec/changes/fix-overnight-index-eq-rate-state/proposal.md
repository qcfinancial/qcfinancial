# Proposal: fix-overnight-index-eq-rate-state

## Why

`OvernightIndexCashflow` computes its equivalent rate in three divergent copies (`getEqRate`, an inline version in `settlementAmount`, and a sentinel-based variant in `amount()`), and each copy honors a different subset of the object's state. This produces real valuation bugs: `amount()` ignores `_gearing`, `record()` ignores `_datesForEquivalentRate`, `amount()` can permanently corrupt `_eqRateDecimalPlaces` on an exception path, and `settlementAmount()` can divide by zero. PV (via `amount()`) and settlement amounts can disagree for the same cashflow.

## What Changes

- Consolidate the equivalent-rate computation into a single parameterized code path (`getEqRate` with explicit decimal places) used by `amount()`, `settlementAmount()`, `_calculateInterest()`, `record()`, and `wrap()`.
- Fix `amount()` to apply `_gearing` (currently `_spread + rate`; every other path uses `eqRate * _gearing + _spread`).
- Fix `record()` (and `OvernightIndexMultiCurrencyCashflow::settlementCurrencyInterest` / `mccyWrap`) to use the index end date for the eq-rate when `_datesForEquivalentRate == qcIndex`, matching `settlementAmount()` and `getRateValue()`.
- Remove the temporary mutation of `_eqRateDecimalPlaces` inside `amount()` (sentinel value 20); pass the "no rounding" intent as a parameter instead so the member is never touched during valuation.
- Add `getEqRate`'s guards (`date <= start` → 0, `yf == 0` → 0, `> 12` decimal places → no rounding) to the `settlementAmount()` path by routing it through the consolidated function.
- Change the 1-arg `accruedInterest(date, indexValue)` and TimeSeries overloads so that, when called with a date on or after the accrual end date, the eq-rate is evaluated at the index end date in `qcIndex` mode (making direct calls at period end agree with `record()`/`wrap()`/`settlementAmount()`); strictly mid-period the documented single-date behavior is unchanged. The TimeSeries overload then looks up the fixing at the eq-rate date. Binder docstrings updated accordingly.
- Add regression tests for the two state combinations current tests do not exercise: `gearing != 1` and `qcIndex` mode with index dates != accrual dates.
- Version bump in `setup.py` and matching commit message per project convention.

Numbers change for existing users in these cases (bug fixes, not breaking API): cashflows with `gearing != 1` priced via `amount()`; `record()`/multi-currency interest in `qcIndex` mode when index dates differ from accrual dates; and 1-arg/TimeSeries `accrued_interest` called at or after the accrual end date in `qcIndex` mode (now consistent with `record()`/`wrap()`).

## Capabilities

### New Capabilities

- `overnight-index-cashflow`: Equivalent-rate computation semantics for `OvernightIndexCashflow` and its multi-currency subclass — single source of truth for the eq-rate, consistent application of gearing/spread, date selection per `DatesForEquivalentRate`, rounding rules, degenerate-period guards, and valuation-method purity (no persistent state mutation during valuation).

### Modified Capabilities

<!-- none — no existing spec covers overnight index cashflows -->

## Impact

- `include/cashflows/OvernightIndexCashflow.h` — signature of the consolidated eq-rate helper (private/protected overload taking decimal places).
- `source/cashflows/OvernightIndexCashflow.cpp` — `amount()`, `settlementAmount()`, `record()`, `wrap()`, `getEqRate()`.
- `source/cashflows/OvernightIndexMultiCurrencyCashflow.cpp` — `settlementCurrencyInterest()`, `mccyWrap()` (inherit the date fix via `_calculateInterest` or the consolidated helper).
- `Tests/` — new/updated Catch2 tests for `OvernightIndexCashflow`.
- No Python binding signature changes; `qcfinancial` users see corrected numbers only in the buggy state combinations listed above.
- `setup.py` version bump.
