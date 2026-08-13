## Context

`PresentValue::pv(cashflow, curve)` (`include/present_value/PresentValue.h:140-166`) computes `_derivatives[i] = curve->dfDerivativeAt(i) * amount`, which assumes `amount` is constant with respect to the discounting curve. That assumption breaks the moment `amount` itself is produced by projecting a curve-dependent forward FX rate — which is exactly what settlement-currency PV needs. A close, working precedent already exists for "project a value via covered interest parity (CIP) from two curves with full per-vertex derivative propagation": `ForwardRates::setRateIcpClfCashflow` (`include/present_value/ForwardRates.h:474-625`), which projects a forward UF (CLF) value from a CLP curve and a UF curve as `ufValuationDate * dfUFCLF(t) / dfUFCLP(t)`. This design generalizes that pattern from a domestic nominal/real pair to a genuine two-currency pair, for `FixedRateMultiCurrencyCashflow` and `IborMultiCurrencyCashflow`.

`FixedRateMultiCurrencyCashflow`/`IborMultiCurrencyCashflow` already carry `_fxRateIndexValue` (set via `setFxRateIndexValue`, currently populated only from a historical-fixing lookup via `ForwardFXRates::setFXRate`/`FXRateEstimator`) and two settlement-amount accessors: `settlementAmount()` (rounds via `QCCurrency::amount()`, `round(x*factor)/factor`) and `settlementCurrencyAmount()` (identical FX conversion, no rounding). Neither currently tracks any derivative.

## Goals / Non-Goals

**Goals:**
- Project a forward FX rate via CIP (`Forward(t) = Spot * DF_notional(t) / DF_settlement(t)`) from two curves, honoring the existing "don't re-project an already-fixed rate" convention used elsewhere in the codebase.
- Cache curve-vertex derivatives of that forward, and of the resulting settlement-currency amount, on the cashflow object — following the codebase's established idiom (`IborCashflow::_amountDerivatives`, `CompoundedOvernightRateCashflow2::_amountDerivatives`, `IcpClfCashflow`'s paired ICP/UF derivative members) rather than inventing a new one.
- Discount that cached, unrounded settlement-currency amount and expose per-curve PV derivatives (to both the notional and settlement curves) plus a leg-accumulated FX delta, via a new `PresentValueFX` class.
- Handle the `FXRateIndex` strong/weak currency-pair asymmetry correctly: it changes the PV's homogeneity degree in Spot and whether the settlement curve's derivative genuinely cancels.

**Non-Goals:**
- `OvernightIndexMultiCurrencyCashflow` / `CompoundedOvernightRateMultiCurrencyCashflow2` support (deferred; same pattern applies later).
- An additive forward-points/cross-currency-basis curve convention (explicitly rejected in favor of the two-curve multiplicative CIP form, which has a direct precedent in this codebase and yields a closed-form FX delta).
- Changing `PresentValue`, `ForwardFXRates::setFXRate`/`setFXRateForLeg` (historical-fixing path), or `settlementAmount()` — all untouched.

## Decisions

**D1 — Three-stage pipeline (projection → cached amount-derivatives → discounting), not one monolithic class.**
Mirrors the codebase's existing split between a `ForwardRates`-style projection step (writes curve-dependent values *and* their derivatives onto the cashflow) and a `PresentValue`-style discounting step (reads `amount()`, combines with the discount curve's own derivative). Alternative considered: compute everything inline inside a single new PV class, reading two curves and spot directly. Rejected per explicit user feedback during design review — derivative bookkeeping belongs on the cashflow object, consistent with `IborCashflow::amount()` caching `_amountDerivatives` from `_forwardRateDerivatives`, not inside the discounting class. This also means a leg mixing already-fixed and still-floating cashflows "just works": the projection step already zeroes derivatives for fixed cashflows, so the discounting step needs no fixed/floating branch of its own.

**D2 — Differentiate through `settlementCurrencyAmount()`, never `settlementAmount()`.**
`settlementAmount()` rounds via `QCCurrency::amount()` (`round(x*factor)/factor`), which is a step function — locally flat almost everywhere, discontinuous at rounding boundaries, and not meaningfully differentiable. `settlementCurrencyAmount()` performs the identical FX conversion with no rounding, already exists on both target classes, and is already bound in the active Python binder. No new unrounded accessor is needed; the fix is to build all derivative caching on top of the existing unrounded method.

**D3 — CIP forward projection extends `ForwardFXRates`, not a new class.**
`ForwardFXRates` already owns "set the FX rate value onto a multi-currency cashflow." Adding `setFXRateCIP`/`setFXRateForLegCIP` as siblings to the existing `setFXRate`/`setFXRateForLeg` keeps that responsibility in one place and mirrors how `ForwardRates` hosts multiple `setRateXxxCashflow` methods for different cashflow/rate types.

**D4 — `PresentValueFX` is a new class parallel to `PresentValue`, not new overloads on it.**
`PresentValue`'s existing derivative-vector shape (one vector, one curve) doesn't fit a two-curve, curve-dependent-amount case without reshaping its public contract. A new class avoids disturbing `PresentValue`'s existing consumers and keeps `PresentValueFX`'s derivative math (a product-rule combination of cached amount-derivatives with the settlement curve's `dfDerivativeAt`) self-contained. `PresentValueFX` deliberately does not call `cashflow->savePresentValue()/saveDiscountFactor()` — those fields feed `record()`'s established notional-currency convention, and overwriting them with a settlement-currency figure would silently corrupt anything reading `record()` after a `PresentValue` pass.

**D5 — Strong/weak branch is load-bearing, not a detail.**
`FXRateIndex::convert()` (`include/asset_classes/FXRateIndex.h:87-103`) multiplies when the cashflow's currency is the pair's strong side, divides when weak. Worked through by hand:
- Strong: `PV = notionalAmount * Forward(t) * DF_settlement(t)`, homogeneous **+1** in Spot. Since `Forward` is built from the *same two curves* used for final discounting, `DF_settlement(t)` algebraically cancels (`PV = notionalAmount * Spot * DF_notional(t)`) — the settlement curve's PV derivative is genuinely zero, a real identity. FX delta = `PV / Spot`.
- Weak: `PV = notionalAmount * DF_settlement(t)^2 / (Spot * DF_notional(t))`, homogeneous **−1** in Spot. Nothing cancels; the settlement curve's PV derivative is structurally nonzero. FX delta = `−PV / Spot`.
The implementation must branch on strong/weak per cashflow and must never hardcode "settlement curve derivative = 0" — it must fall out of the arithmetic, not be special-cased, both for correctness in the weak branch and so a future settlement curve that isn't perfectly CIP-consistent with the notional curve still gets a correct nonzero risk number.

**D6 — Leg-level FX delta is a per-cashflow sum, never `PV_leg / Spot`.**
A leg commonly mixes cashflows whose FX rate has already fixed (zero FX sensitivity — Spot no longer affects their settlement amount) with cashflows still floating. `PV_leg / Spot` is only valid when every cashflow in the leg is still floating and strong-side; the general, always-correct computation accumulates each cashflow's own FX delta.

## Risks / Trade-offs

- **[Risk] A caller uses `settlementAmount()` instead of `settlementCurrencyAmount()` somewhere in this pipeline, silently invalidating derivatives.** → Mitigation: derivative caching lives inside `settlementCurrencyAmount()` itself; `PresentValueFX::pv()` calls that method by name, and a test asserts the two methods' outputs differ (proving the rounded one wasn't accidentally used) whenever rounding is non-trivial for the test currency pair.
- **[Risk] Someone "simplifies" the strong-branch settlement-curve derivative to a hardcoded zero, which is correct only when the settlement curve used for CIP projection is literally the same object as the one used for final discounting.** → Mitigation: the implementation computes it via the same product-rule formula in both branches (never a special-cased zero-fill for strong), and a regression test checks the weak-branch settlement-curve derivative is nonzero.
- **[Risk] `PresentValueFX` mutating `record()`-visible fields (`present_value`/`discount_factor`) would corrupt downstream consumers that read `record()` after a mixed `PresentValue`/`PresentValueFX` pricing pass.** → Mitigation: `PresentValueFX` never calls `savePresentValue()`/`saveDiscountFactor()`; results are read only through `PresentValueFX`'s own accessors (see D4).
- **[Trade-off] CIP forward assumes the settlement curve is (or is meant to approximate) the true funding curve implied by the notional curve and spot; it does not model an independently-quoted cross-currency basis.** Accepted per user decision — matches the existing `IcpClfCashflow` precedent and needs no new curve type. A future change can introduce a basis-adjusted forward if needed, using the same two-stage architecture.

## Migration Plan

Purely additive — no existing class's behavior changes. New methods/classes are opt-in. Version bump `1.13.0` → `1.14.0` in `setup.py`, `CLAUDE.md`, and `qcf_binder.cpp`'s `id()` string, per this repo's checklist. No rollback concerns beyond a normal revert (no data migrations, no changed wire formats).

## Open Questions

- None outstanding for this pass. Extending to `OvernightIndexMultiCurrencyCashflow`/`CompoundedOvernightRateMultiCurrencyCashflow2` and to an independently-quoted forward-points/basis curve are both explicitly deferred, not open — see Non-Goals.
