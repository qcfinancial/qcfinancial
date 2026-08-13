## MODIFIED Requirements

### Requirement: Settlement-currency present value with per-curve derivatives and FX delta
`PresentValueFX::pv` SHALL discount a cashflow's (or leg's) `settlementCurrencyAmount()` using the supplied discount curve, and SHALL expose, via `getNotionalCurveDerivatives()`/`getCipSettlementCurveDerivatives()`/`getDiscountCurveDerivatives()`/`getFxDelta()`, present-value derivatives with respect to every vertex of the notional curve, the CIP-projection curve (the curve previously supplied to `ForwardFXRates::setFXRateCIP` when the cashflow's forward FX rate was projected), and the discount curve, plus an FX delta. `getCipSettlementCurveDerivatives()` SHALL be computed from the cashflow's cached amount-derivatives alone (the CIP-projection curve's contribution), and `getDiscountCurveDerivatives()` SHALL be computed from the discount curve's own discount-factor derivative alone (the discount curve's contribution); neither method SHALL combine the two. The two vectors SHALL each be sized to their own curve's length independently, with no assumption that the CIP-projection curve and the discount curve share a node set, a length, or are the same object. `PresentValueFX` SHALL NOT modify the cashflow's `present_value`/`discount_factor` fields used by `record()`.

#### Scenario: Matured cashflow contributes zero
- **WHEN** the valuation date is on or after the cashflow's end date
- **THEN** `pv()` returns `0.0` and all derivative vectors and the FX delta are zero

#### Scenario: Strong-side settlement contributions cancel only when summed
- **WHEN** `pv()` is computed for a still-floating, strong-side-notional cashflow using the same curve object for both the CIP projection (passed earlier to `setFXRateCIP`) and discounting (passed to `pv()`)
- **THEN** `getCipSettlementCurveDerivatives()` and `getDiscountCurveDerivatives()` are each generally non-zero, their vertex-by-vertex sum is zero at every vertex, and `pv()` equals the notional amount times the spot value times the notional curve's discount factor at the cashflow's end date

#### Scenario: Distinct CIP-projection and discount curves produce independent, well-defined vectors
- **WHEN** `pv()` is computed for a still-floating cashflow whose forward FX rate was projected (via `setFXRateCIP`) off one curve, and `pv()` is called with a different curve (different node count) as the discount curve
- **THEN** `getCipSettlementCurveDerivatives()` is sized to the CIP-projection curve's length, `getDiscountCurveDerivatives()` is sized to the discount curve's length, both are computed without error, and `pv()`'s returned value uses the CIP-projected forward amount discounted by the given discount curve's discount factor

#### Scenario: Weak-side settlement curve derivatives are non-zero
- **WHEN** `pv()` is computed for a still-floating, weak-side-notional cashflow using the same curve object for CIP projection and discounting
- **THEN** the sum of `getCipSettlementCurveDerivatives()` and `getDiscountCurveDerivatives()` is non-zero for at least one vertex where that curve's own discount-factor derivative is non-zero

#### Scenario: FX delta sign follows strong/weak side
- **WHEN** `pv()` is computed for a still-floating cashflow
- **THEN** the FX delta equals `+pv()/Spot` when the notional currency is the strong side, and `-pv()/Spot` when it is the weak side

#### Scenario: Already-fixed cashflow has zero FX delta
- **WHEN** `pv()` is computed for a cashflow whose FX rate has already fixed
- **THEN** the FX delta is zero, the notional-curve present-value derivative is zero at every vertex, `getCipSettlementCurveDerivatives()` is all zeros, and `getDiscountCurveDerivatives()` is non-zero (ordinary discounting of a fixed amount)

#### Scenario: Leg-level FX delta is a per-cashflow sum
- **WHEN** `pv()` is computed for a `Leg` containing at least one already-fixed and at least one still-floating cashflow
- **THEN** the leg's FX delta equals the sum of the FX deltas each cashflow would produce individually, and is not equal to `pv() / Spot`

#### Scenario: Unsupported cashflow type is rejected
- **WHEN** `pv()` is called with a cashflow type other than `FixedRateMultiCurrencyCashflow` or `IborMultiCurrencyCashflow`
- **THEN** the call throws `std::invalid_argument`

## REMOVED Requirements

### Requirement: Combined settlement-curve derivative getter
**Reason**: `getSettlementCurveDerivatives()` summed the CIP-projection curve's and the discount curve's contributions into one vector, indexed by a shared position `j`. This is only meaningful when the two curves are the same object (same node set) — `PresentValueFX::pv` never enforced that, so passing genuinely distinct curves produced either an out-of-bounds access or a silently-wrong number. Replaced by two independent getters (see `getCipSettlementCurveDerivatives`/`getDiscountCurveDerivatives` in the MODIFIED requirement above) that are always well-defined regardless of whether the two curves coincide.
**Migration**: Callers that always used the same curve object for both `setFXRateCIP` and `pv()` (the only usage this method ever supported correctly) should call `getCipSettlementCurveDerivatives()` and `getDiscountCurveDerivatives()` and sum the two vectors themselves.
