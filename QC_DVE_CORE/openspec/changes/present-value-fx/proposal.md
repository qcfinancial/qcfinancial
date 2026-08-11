## Why

`PresentValue::pv` only discounts a cashflow in its own notional currency. There is no way to price a `FixedRateMultiCurrencyCashflow`/`IborMultiCurrencyCashflow` (or a `Leg` of them) in a *different* settlement currency by projecting a forward FX rate and discounting the converted amount, and the library has no notion of "FX delta" (PV sensitivity to spot) at all. Cross-currency and quanto-style products need this to be risk-managed.

## What Changes

- Add `ForwardFXRates::setFXRateCIP` / `setFXRateForLegCIP`: project a forward FX rate via covered interest parity (CIP), `Forward(t) = Spot * DF_notional(t) / DF_settlement(t)`, from a notional-currency curve and a settlement-currency curve, and cache that forward's per-curve-vertex derivatives on the cashflow. Skips projection (leaves the stored fixing untouched) once `valuationDate` is past the FX fixing date, mirroring `ForwardRates::setRateIborCashflow1`.
- Add curve-vertex FX-forward derivative members (`_fxRateNotionalCurveDerivatives`, `_fxRateSettlementCurveDerivatives`) to `FixedRateMultiCurrencyCashflow` and `IborMultiCurrencyCashflow`, populated by the projection step above.
- Extend `settlementCurrencyAmount()` on both classes to cache curve-vertex amount derivatives (`_amountNotionalCurveDerivatives`, `_amountSettlementCurveDerivatives`) and an FX-spot amount derivative (`_amountFxDelta`), computed by chain rule from the cached FX-forward derivatives. `settlementAmount()` (which rounds) is untouched and must not be used for this.
- Add `FixedRateMultiCurrencyCashflow::getFxRateIndexValue()` (small existing asymmetry vs. `IborMultiCurrencyCashflow`, which already has it).
- Add a new class `PresentValueFX` (parallel to `PresentValue`): discounts a cashflow's/leg's `settlementCurrencyAmount()` with the settlement curve, combining the cashflow's cached amount-derivatives with the settlement curve's own discount-factor derivative via the product rule, and exposes per-curve-vertex PV derivatives (to both notional and settlement curves) plus a leg-accumulated FX delta.
- Expose all of the above through `source/qcf_binder.cpp` (the sole active binder).
- Version bump `1.13.0` → `1.14.0` (three places: `setup.py`, `CLAUDE.md`, `qcf_binder.cpp`'s `id()` string), per this repo's versioning convention.

## Capabilities

### New Capabilities
- `settlement-currency-present-value`: present-valuing a multi-currency cashflow/leg in its settlement currency via a CIP-projected forward FX rate, with per-curve PV derivatives and FX delta.

### Modified Capabilities
(none — no existing spec's requirements change)

## Impact

- **Affected code**: `include/present_value/ForwardFXRates.h`/`.cpp`, `include/present_value/PresentValueFX.h` (new), `source/present_value/PresentValueFX.cpp` (new), `include/cashflows/FixedRateMultiCurrencyCashflow.h`/`.cpp`, `include/cashflows/IborMultiCurrencyCashflow.h`/`.cpp`, `source/qcf_binder.cpp`, `source/CMakeLists.txt`, `Tests/PresentValueFXTests.cpp` (new), `setup.py`, `CLAUDE.md`.
- **Scope**: `FixedRateMultiCurrencyCashflow` and `IborMultiCurrencyCashflow` only in this pass. `OvernightIndexMultiCurrencyCashflow`/`CompoundedOvernightRateMultiCurrencyCashflow2` are explicitly deferred to a later change, following the same pattern.
- **No breaking changes**: purely additive — existing `PresentValue`, `ForwardFXRates::setFXRate`/`setFXRateForLeg` (historical-fixing path), and `settlementAmount()` are untouched.
