## Why

`PresentValueFX::pv` sums two curve-vertex derivative terms into one `getSettlementCurveDerivatives()` vector: the CIP-projection curve's contribution (cached on the cashflow by `ForwardFXRates::setFXRateCIP`) and the discount curve's own `dfDerivativeAt` contribution. This is only correct when the two curves are the literal same object — nothing in the API enforces that, and `PresentValueFX::pv` accepts any `InterestRateCurve` as its discounting curve independent of what was passed to `setFXRateCIP` earlier. When a caller legitimately projects the forward off one curve and discounts off a different curve of the same currency (e.g. two distinct CLP curves), the sum either throws (shorter curve's `.at(j)` out of range) or silently zips two unrelated curves' node-derivatives together under one index, producing a meaningless number. This was a known, accepted risk at design time (`present-value-fx`'s `design.md`, decision D5) but the API gives no way to get correct, independent sensitivities in that case.

## What Changes

- **BREAKING**: `PresentValueFX::getSettlementCurveDerivatives()` is removed. The CIP-projection-curve piece and the discount-curve piece are exposed as two separate, always-independent vectors instead of one combined one.
- Add `PresentValueFX::getCipSettlementCurveDerivatives()` — PV derivative w.r.t. the curve that produced the cashflow's cached CIP forward-FX derivatives (the amount-side term only).
- Add `PresentValueFX::getDiscountCurveDerivatives()` — PV derivative w.r.t. the curve `pv()` actually discounts with (the discount-factor-side term only).
- Rename the `settlementCurve` parameter of `PresentValueFX::pv()` to `discountCurve` (header-only; the pybind11 binding is positional, so this is not a binder break).
- Python binder: `get_settlement_curve_derivatives` removed; `get_cip_settlement_curve_derivatives` and `get_discount_curve_derivatives` added.
- No change to `ForwardFXRates::setFXRateCIP`/`setFXRateForLegCIP` or to the cashflow-level caching (`_amountNotionalCurveDerivatives`, `_amountSettlementCurveDerivatives`, `_amountFxDelta`) — that layer is already correctly scoped to the CIP curve alone.
- Callers who rely on the two curves coinciding (the common case, including every current test) now sum `getCipSettlementCurveDerivatives() + getDiscountCurveDerivatives()` themselves to recover the old combined number; the two vectors have independent lengths so no elementwise mismatch is possible.
- Version bump `1.14.0a1` → `1.14.0a2` (still pre-release; this change lands as another alpha increment on the same `1.14.0` line, not the final release).

## Capabilities

### New Capabilities
(none)

### Modified Capabilities
- `settlement-currency-present-value`: the "Settlement-currency present value with per-curve derivatives and FX delta" requirement changes its derivative-vector shape from one combined settlement-curve vector to two independent vectors (CIP-projection curve, discount curve). The CIP-projection requirement (`ForwardFXRates::setFXRateCIP`) and the amount-derivative-caching requirement are untouched.

  Note: this capability's spec was authored under the `present-value-fx` change, which is complete but not yet archived into `openspec/specs/` — this change's spec delta is written against that pending baseline.

## Impact

- **Affected code**: `include/present_value/PresentValueFX.h`, `source/present_value/PresentValueFX.cpp`, `source/qcf_binder.cpp` (binder methods + `id()` version string), `setup.py`, `CLAUDE.md`.
- **Affected tests**: `Tests/PresentValueFXTests.cpp` — 4 existing `REQUIRE`s against `getSettlementCurveDerivatives()` rewritten to sum the two new getters (regression proof the split is numerically consistent with prior same-curve behavior); new tests added for the two-distinct-curves case (including mismatched node counts).
- **No changes** to `ForwardFXRates`, `FixedRateMultiCurrencyCashflow`/`IborMultiCurrencyCashflow` caching, or `PresentValue` (notional-currency path).
- **Breaking, pre-release**: `present-value-fx` has not been tagged/released past `1.14.0a1`, so there are no external consumers of the removed getter to migrate.
