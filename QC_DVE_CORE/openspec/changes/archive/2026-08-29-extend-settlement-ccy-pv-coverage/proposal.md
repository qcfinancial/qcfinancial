## Why

`settlement-currency-present-value` (shipped in 1.14.0a2) only covers `FixedRateMultiCurrencyCashflow` and `IborMultiCurrencyCashflow`. Three more multi-currency cashflow types carry the same settlement-currency need but have no CIP-forward-projection or `PresentValueFX` support: `OvernightIndexMultiCurrencyCashflow` and `CompoundedOvernightRateMultiCurrencyCashflow2` are missing the CIP-forward and PV legs (spot-fixing already works), and `SimpleMultiCurrencyCashflow` — the type used for NDFs — has no integration at all (not even spot-fixing). Without this, none of these types can be present-valued in settlement currency with curve-vertex derivatives and FX delta, which blocks NDF pricing and floating-rate cross-currency products from using the same risk pipeline as fixed/Ibor cross-currency swaps.

## What Changes

- Add CIP-forward-projection support (`ForwardFXRates::setFXRateCIP`) and `PresentValueFX` support for `OvernightIndexMultiCurrencyCashflow` and `CompoundedOvernightRateMultiCurrencyCashflow2`, mirroring the existing `FixedRateMultiCurrencyCashflow`/`IborMultiCurrencyCashflow` pattern: 4 setters + 3 getters + curve-vertex derivative caching in `settlementCurrencyAmount()`.
- Add a `settlementCurrencyAmount()` method to `SimpleMultiCurrencyCashflow` (behavior-preserving refactor of the conversion math currently inline in `settlementAmount()`, plus strong/weak-side derivative caching), and wire it into all three integration points: `ForwardFXRates::setFXRate` (spot fixing — currently entirely absent for this type), `ForwardFXRates::setFXRateCIP`, and `PresentValueFX::pv`.
- Extend Python bindings (`source/qcf_binder.cpp`) for the new methods on all three types.
- Extend `Tests/PresentValueFXTests.cpp` with scenarios for the three newly-supported types, mirroring the existing Fixed/Ibor scenario coverage.
- Add a marimo notebook and an Excel workbook (closed-form formulas only, no VBA) for human review, covering spot-fixing, CIP-forward projection, and `PresentValueFX` (PV + all curve derivatives + FX delta) for all five multi-currency cashflow types (Fixed, Ibor, Overnight, CompoundedOvernight, Simple/NDF).
- Add an in-project `uv`-managed Python 3.14 virtual environment to run the marimo notebook.
- `IcpClfCashflow` is explicitly out of scope: it has no `FXRateIndex`/spot FX, settlement is hardcoded to CLP, and it already caches its own three curve-vertex derivative sets directly in `amount()` — it does not fit the CIP-forward pattern this change extends.

## Capabilities

### New Capabilities

(none — this change widens an existing capability's requirements; the review artifacts and dev environment are implementation deliverables, not new library behavior, and are covered in tasks.md)

### Modified Capabilities

- `settlement-currency-present-value`: widen the CIP-forward-projection and `PresentValueFX` requirements to cover `OvernightIndexMultiCurrencyCashflow`, `CompoundedOvernightRateMultiCurrencyCashflow2`, and `SimpleMultiCurrencyCashflow`, in addition to the two types already supported.

## Impact

- `include/cashflows/OvernightIndexMultiCurrencyCashflow.h`, `source/cashflows/OvernightIndexMultiCurrencyCashflow.cpp`
- `include/cashflows/CompoundedOvernightRateMultiCurrencyCashflow2.h`, `source/cashflows/CompoundedOvernightRateMultiCurrencyCashflow2.cpp`
- `include/cashflows/SimpleMultiCurrencyCashflow.h`, `source/cashflows/SimpleMultiCurrencyCashflow.cpp`
- `source/present_value/ForwardFXRates.cpp` (both `setFXRate` and `setFXRateCIP` dispatch)
- `source/present_value/PresentValueFX.cpp` (`pv` dispatch)
- `source/qcf_binder.cpp`, `include/QcfinancialPybind11Helpers.h`
- `Tests/PresentValueFXTests.cpp`
- New: marimo notebook, Excel workbook, `pyproject.toml`/`uv.lock` for the Python 3.14 review environment
- Version bump (`setup.py`, `CLAUDE.md`, `qcf_binder.cpp` id string) per project convention
- No breaking changes: all additions are new methods/branches; existing behavior of the five cashflow types is unchanged (the `SimpleMultiCurrencyCashflow::settlementAmount()` refactor is numerically identical to today's behavior)
