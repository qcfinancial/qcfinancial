## 1. Cashflow-level derivative members

- [x] 1.1 Add `double getFxRateIndexValue() const;` to `FixedRateMultiCurrencyCashflow` (header + `.cpp`), mirroring the existing `IborMultiCurrencyCashflow::getFxRateIndexValue()`
- [x] 1.2 Add `_fxRateNotionalCurveDerivatives`, `_fxRateSettlementCurveDerivatives` (vector<double>) members and `setFxRateNotionalCurveDerivatives()`/`setFxRateSettlementCurveDerivatives()` setters to `FixedRateMultiCurrencyCashflow` and `IborMultiCurrencyCashflow` (also added `_fxRateSpotDerivative`/`setFxRateSpotDerivative()`, needed so `_amountFxDelta` can be chain-ruled without `settlementCurrencyAmount()` taking a spot parameter)
- [x] 1.3 Add `_amountNotionalCurveDerivatives`, `_amountSettlementCurveDerivatives` (vector<double>) and `_amountFxDelta` (double) members with `getAmountNotionalCurveDerivatives()`/`getAmountSettlementCurveDerivatives()`/`getAmountFxDelta()` getters to both classes
- [x] 1.4 Extend `settlementCurrencyAmount()` on both classes to compute and cache the three members from 1.3 via chain rule off the FX-forward derivatives from 1.2, branching on `FXRateIndex` strong/weak side (per design.md D5); never touch `settlementAmount()`

## 2. CIP forward FX projection

- [x] 2.1 Add `ForwardFXRates::setFXRateCIP(valuationDate, spotFxValue, cashflow, notionalCurve, settlementCurve)`: dispatch on `getType()`/`dynamic_cast` (Fixed/Ibor multi-currency only, `throw std::invalid_argument` otherwise), skip projection and zero both FX-forward derivative vectors when past the FX fixing date (or matured), otherwise compute `Forward(t) = Spot * DF_notional(t) / DF_settlement(t)` and its two curve-vertex derivative vectors + spot derivative, call `setFxRateIndexValue` and the setters from 1.2. Shared math factored into a private template helper `_projectFXRateCIP` to avoid duplicating it per type. Follows `setFXRate`'s copy-and-return-shared_ptr convention (mutates a local copy, does not mutate the reference argument in place).
- [x] 2.2 Add `ForwardFXRates::setFXRateForLegCIP(valuationDate, spotFxValue, leg, notionalCurve, settlementCurve)` looping 2.1 over a `Leg`

## 3. PresentValueFX

- [x] 3.1 Create `include/present_value/PresentValueFX.h`: class `QCode::Financial::PresentValueFX` with `pv(valuationDate, cashflow, settlementCurve)`, `pv(valuationDate, leg, settlementCurve)`, `getNotionalCurveDerivatives()`, `getSettlementCurveDerivatives()`, `getFxDelta()`
- [x] 3.2 Create `source/present_value/PresentValueFX.cpp`: single-cashflow `pv()` reads `cf.settlementCurrencyAmount()` (never `settlementAmount()`), returns `0.0` with zeroed derivatives/delta when matured, otherwise discounts with the settlement curve and combines the cashflow's cached amount-derivatives with `settlementCurve->dfDerivativeAt(j)` via product rule (per design.md); does not call `savePresentValue()`/`saveDiscountFactor()`
- [x] 3.3 Implement the leg `pv()` overload: accumulate result, both derivative vectors, and FX delta cashflow-by-cashflow (never `PV_leg / Spot`)

## 4. Build wiring

- [x] 4.1 Add `present_value/PresentValueFX.cpp` to `target_sources(QC_DVE_CORE ...)` in `source/CMakeLists.txt` (new `source/present_value/` subdirectory)

## 5. Python bindings

- [x] 5.1 `#include <present_value/PresentValueFX.h>` in `source/qcf_binder.cpp` next to the other `present_value/*.h` includes
- [x] 5.2 Extend the existing `ForwardFXRates` binder block with `set_fx_rate_cip` / `set_fx_rate_for_leg_cip`
- [x] 5.3 Register `py::class_<qf::PresentValueFX>` next to the existing `PresentValue` block: `py::init<>()`, both `pv` overloads via `py::overload_cast`, the three getters, `py::pickle` with an empty-tuple state
- [x] 5.4 Expose the new `FixedRateMultiCurrencyCashflow`/`IborMultiCurrencyCashflow` getters/setters from Section 1 in their existing `py::class_<...>` binder blocks

## 6. Tests

- [x] 6.1 Create `Tests/PresentValueFXTests.cpp` following `Tests/PresentValueTests.cpp`/`Tests/FXRateTests.cpp` patterns: two curves with different rates, one `FixedRateMultiCurrencyCashflow` and one `IborMultiCurrencyCashflow`, each built strong-notional and weak-notional (zero-rate cashflows, so `amount() == nominal` exactly and expected values are easy to hand-verify)
- [x] 6.2 Test: strong-branch cancellation identity — `pv() == notionalAmount * spot * DF_notional(t)`, settlement-curve PV-derivatives ≈ 0
- [x] 6.3 Test: weak-branch settlement-curve PV-derivatives are not ≈ 0
- [x] 6.4 Test: finite-difference check of `getFxDelta()` (bump spot by `1e-6`, rerun projection + pv, compare slope) for both strong (`+pv/Spot`) and weak (`-pv/Spot`) branches
- [x] 6.5 Test: already-fixed cashflow — cashflow's amount-derivatives and FX delta all zero, `PresentValueFX`'s settlement-curve derivative still non-zero
- [x] 6.6 Test: leg mixing one fixed + one floating cashflow — leg FX delta equals sum of individually-computed deltas and is not `pv_leg / spot`
- [x] 6.7 Test: unsupported cashflow type throws `std::invalid_argument` from both `setFXRateCIP` and `PresentValueFX::pv`
- [x] 6.8 Add `PresentValueFXTests.cpp` to the currently-commented-out block in `Tests/CMakeLists.txt`

## 7. Documentation and versioning

- [x] 7.1 Add a one-line bullet for `PresentValueFX`/`ForwardFXRates::setFXRateCIP` under "Layer 6 — Present Value / Pricing" in `CLAUDE.md`
- [x] 7.2 Bump version `1.13.0` → `1.14.0` in `setup.py`, `CLAUDE.md`'s "Current version" line, and the `id()` string in `source/qcf_binder.cpp`

## 8. Verification

- [x] 8.1 Build succeeds: `cmake --build cmake-build-release` — both `QC_DVE_CORE.a` and `qcfinancial.cpython-311-darwin.so` compiled clean after fixing one stray artifact in `qcf_binder.cpp` (an errant `git` token on its own line, unrelated to this change's edits, left over near the `id()` string — removed)
- [x] 8.2 `PresentValueFXTests.cpp` compiled, linked, and run standalone against Catch2 + `QC_DVE_CORE.a` (manual `c++`/`ninja`-less build, since the CMake test target itself is still disabled per CLAUDE.md). All 6 `PresentValueFX` test cases pass (34/34 assertions), and `LegFactoryTests.cpp` passes too (4/4 assertions). Full suite: 36/43 test cases, 120608/120615 assertions pass; the 7 remaining failures (`CompoundedOvernightRateCashflowTests.cpp:118`, four `QCBusinessCalendarTests.cpp` holiday-count checks, `FXRateTests.cpp:32`) are pre-existing and unrelated to this change or to any of the follow-up fixes below — not touched.
  - **Follow-up fix 1 — broken includes**: `deprecated/CompoundedOvernightRateCashflow.h`, `deprecated/CompoundedOvernightRateCashflow.cpp`, `Tests/include/TestHelpers.h`, and `Tests/CompoundedOvernightRateCashflowTests.cpp` all wrote `#include "cashflows/CompoundedOvernightRateCashflow.h"`/`<cashflows/LinearInterestRateCashflow.h>`, but those headers live flat in `deprecated/` (no `cashflows/` subdirectory there). Corrected all four to flat includes and added `deprecated` to the Tests target's (still-disabled) include directories in `Tests/CMakeLists.txt`.
  - **Follow-up fix 2 — stale LegFactory call**: `TestHelpers::getConrLeg` called `LegFactory::buildBulletCompoundedOvernightLeg`, which no longer exists — the live `LegFactory` only builds the current `CompoundedOvernightRateCashflow2`/multi-currency successor via `buildBulletCompoundedOvernightRateLeg2`. Migrated the call (new required `QCInterestRate` param filled with `TestHelpers::getLinAct360()`; also fixed a latent bug where the function ignored its own `startDate` parameter in favor of a hardcoded literal), and updated `Tests/LegFactoryTests.cpp`'s `dynamic_pointer_cast` target from the deprecated `CompoundedOvernightRateCashflow` to `CompoundedOvernightRateCashflow2` to match.
  - **Own bug found and fixed**: `PresentValueFXTests.cpp`'s Ibor strong-notional case failed for real (884005529 vs expected 873350642) because `TestHelpers::getSofr()` returns a shared *static* `InterestRateIndex` mutated in place by other test files (the deprecated `CompoundedOvernightRateCashflow` calls `_index->setRateValue(...)`), so its rate value depends on test execution order — not the zero it was at authoring time. Fixed locally in `buildIborMccyCashflow` by calling `setInterestRateValue(0.0)` right after construction, making the test deterministic regardless of link/run order; did not touch the shared fixture itself given the blast radius across other test files.
- [x] 8.3 From Python (via the built `qcfinancial.cpython-311-darwin.so`): projected and priced both a strong-notional and a weak-notional `FixedRateMultiCurrencyCashflow` plus an already-fixed one. Confirmed: strong-branch `pv()` matches `notional * spot * DF_notional(t)` exactly, its settlement-curve derivatives are ~0 (float noise, ~1e-7), and `get_fx_delta() == pv()/spot` exactly; weak-branch settlement-curve derivatives are non-zero and `get_fx_delta() == -pv()/spot` exactly; already-fixed case leaves the stored FX value untouched, zeroes notional-curve derivatives and FX delta, and keeps a non-zero settlement-curve derivative
