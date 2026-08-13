## 1. PresentValueFX core

- [x] 1.1 In `include/present_value/PresentValueFX.h`: rename `pv()`'s `settlementCurve` parameter to `discountCurve`; replace `_settlementCurveDerivatives`/`getSettlementCurveDerivatives()` with `_cipSettlementCurveDerivatives`/`getCipSettlementCurveDerivatives()` and `_discountCurveDerivatives`/`getDiscountCurveDerivatives()`.
- [x] 1.2 In `source/present_value/PresentValueFX.cpp`: split the single summed loop (`PresentValueFX.cpp:54-57`) into two independent loops, each sized to its own curve's length — `getCipSettlementCurveDerivatives()` = `amountSettlementCurveDerivatives.at(j) * dfDiscount`; `getDiscountCurveDerivatives()` = `settlementCurrencyAmount * discountCurve->dfDerivativeAt(j)`.
- [x] 1.3 Update the `Leg` overload's accumulation loop to accumulate both new vectors independently (mirroring the existing per-cashflow accumulation pattern for `_notionalCurveDerivatives`).

## 2. Python binder

- [x] 2.1 In `source/qcf_binder.cpp`: remove `get_settlement_curve_derivatives`; add `get_cip_settlement_curve_derivatives` and `get_discount_curve_derivatives`.
- [x] 2.2 Bump the `id()` version string to `1.14.0a2`.

## 3. Tests

- [x] 3.1 In `Tests/PresentValueFXTests.cpp`: rewrite the 4 existing `REQUIRE`s against `getSettlementCurveDerivatives()` to instead sum `getCipSettlementCurveDerivatives()` + `getDiscountCurveDerivatives()` (same-curve cases — strong-side cancellation-to-zero, weak-side non-zero).
- [x] 3.2 Add a test with distinct CIP-projection and discount curves of different node counts: assert both getters return correctly-sized, non-throwing, independently-computed vectors, and that `pv()`'s value matches the CIP-projected forward amount discounted by the discount curve's own discount factor.
- [x] 3.3 Add/confirm a test that an already-fixed cashflow yields all-zero `getCipSettlementCurveDerivatives()` and non-zero `getDiscountCurveDerivatives()`.

## 4. Versioning and docs

- [x] 4.1 Bump `version=` in `setup.py` from `1.14.0a1` to `1.14.0a2`.
- [x] 4.2 Update the "Current version" line in `CLAUDE.md` to `1.14.0a2`, and update the `PresentValueFX` description under Layer 6 to mention the split getters instead of the combined one.
- [x] 4.3 Commit message follows `# Update to Version 1.14.0a2: <description>` per repo convention.

## 5. Build and verify

- [x] 5.1 Rebuild the extension (`python setup.py bdist_wheel` or `cmake --build cmake-build-release`) and confirm it compiles clean.
- [x] 5.2 Run/inspect `Tests/PresentValueFXTests.cpp` cases (test executable is currently commented out in `Tests/CMakeLists.txt` — either re-enable per `CLAUDE.md`'s instructions or verify via a scratch Python session against the rebuilt wheel). Verified via scratch Python session against the rebuilt extension: same-curve strong-side sum cancels to ~0 while each piece is individually non-zero, and distinct/mismatched-length CIP/discount curves produce correctly-sized, independently-correct vectors matching the closed-form CIP forward/discount formula.
