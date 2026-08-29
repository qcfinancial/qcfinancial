## MODIFIED Requirements

### Requirement: CIP forward FX projection onto multi-currency cashflows
`ForwardFXRates::setFXRateCIP` SHALL project a forward FX rate for a `FixedRateMultiCurrencyCashflow`, `IborMultiCurrencyCashflow`, `OvernightIndexMultiCurrencyCashflow`, `CompoundedOvernightRateMultiCurrencyCashflow2`, or `SimpleMultiCurrencyCashflow` as `Forward(t) = Spot * DF_notional(t) / DF_settlement(t)`, where `t` is the valuation date's day difference to the cashflow's end date, `DF_notional`/`DF_settlement` are discount factors from the supplied notional-currency and settlement-currency curves, and SHALL cache the forward's derivative with respect to every vertex of both curves on the cashflow. `ForwardFXRates::setFXRateForLegCIP` SHALL apply this to every cashflow in a `Leg`. Cashflow types other than the five listed SHALL cause `std::invalid_argument`.

#### Scenario: Forward FX projected for a still-floating cashflow
- **WHEN** `setFXRateCIP` is called with a valuation date on or before the cashflow's FX fixing date
- **THEN** the cashflow's FX rate index value is set to `Spot * DF_notional(t) / DF_settlement(t)`, and its notional-curve and settlement-curve FX-forward derivative vectors are populated (non-zero for at least one vertex when the corresponding curve is not flat at that vertex)

#### Scenario: Already-fixed cashflow is left untouched
- **WHEN** `setFXRateCIP` is called with a valuation date after the cashflow's FX fixing date
- **THEN** the cashflow's stored FX rate index value is unchanged, and both FX-forward derivative vectors are set to all zeros

#### Scenario: Overnight-indexed and compounded-overnight-rate cashflows project the same forward
- **WHEN** `setFXRateCIP` is called on a still-floating `OvernightIndexMultiCurrencyCashflow` or `CompoundedOvernightRateMultiCurrencyCashflow2` with a notional curve and a settlement curve
- **THEN** the projected forward and its cached curve-vertex derivatives match the same CIP formula and derivative scenarios defined above for `FixedRateMultiCurrencyCashflow`/`IborMultiCurrencyCashflow`

#### Scenario: NDF-shaped cashflow projects the same forward
- **WHEN** `setFXRateCIP` is called on a still-floating `SimpleMultiCurrencyCashflow`
- **THEN** the projected forward and its cached curve-vertex derivatives match the same CIP formula and derivative scenarios defined above for `FixedRateMultiCurrencyCashflow`/`IborMultiCurrencyCashflow`

#### Scenario: Unsupported cashflow type is rejected
- **WHEN** `setFXRateCIP` is called with a multi-currency cashflow type other than `FixedRateMultiCurrencyCashflow`, `IborMultiCurrencyCashflow`, `OvernightIndexMultiCurrencyCashflow`, `CompoundedOvernightRateMultiCurrencyCashflow2`, or `SimpleMultiCurrencyCashflow`
- **THEN** the call throws `std::invalid_argument`

### Requirement: Curve-vertex amount derivatives cached without rounding
`settlementCurrencyAmount()` on `FixedRateMultiCurrencyCashflow`, `IborMultiCurrencyCashflow`, `OvernightIndexMultiCurrencyCashflow`, `CompoundedOvernightRateMultiCurrencyCashflow2`, and `SimpleMultiCurrencyCashflow` SHALL, on every call, cache curve-vertex derivatives of the returned (unrounded) settlement-currency amount with respect to both the notional and settlement curves, and a derivative with respect to the spot FX value, derived by chain rule from the cashflow's cached FX-forward derivatives. The rounding method `settlementAmount()` SHALL NOT be used as the basis for any derivative computation, and where a type's `settlementAmount()` returns an unrounded value (as `SimpleMultiCurrencyCashflow` does today), introducing `settlementCurrencyAmount()` SHALL NOT change the numeric value `settlementAmount()` returns.

#### Scenario: Strong-side notional currency
- **WHEN** the cashflow's notional currency is the strong side of its `FXRateIndex` pair and its FX-forward derivatives are non-zero
- **THEN** `settlementCurrencyAmount()`'s cached amount-derivatives equal the notional amount times the corresponding cached FX-forward derivative, and the cached FX-spot amount-derivative equals the settlement-currency amount divided by the spot value

#### Scenario: Weak-side notional currency
- **WHEN** the cashflow's notional currency is the weak side of its `FXRateIndex` pair and its FX-forward derivatives are non-zero
- **THEN** `settlementCurrencyAmount()`'s cached amount-derivatives and cached FX-spot amount-derivative carry the opposite sign and the inverse-square scaling implied by `settlementCurrencyAmount = notionalAmount / Forward`

#### Scenario: Already-fixed cashflow yields zero amount-derivatives
- **WHEN** `settlementCurrencyAmount()` is called on a cashflow whose FX-forward derivatives were zeroed by the already-fixed projection path
- **THEN** the cached amount-derivatives with respect to both curves and to spot are all zero

#### Scenario: SimpleMultiCurrencyCashflow settlementAmount() is unchanged
- **WHEN** `settlementCurrencyAmount()` is added to `SimpleMultiCurrencyCashflow` and `settlementAmount()` is refactored to call it
- **THEN** `settlementAmount()` returns the same numeric value it returned before the refactor, for the same nominal, FX rate index value, and currency pairing

### Requirement: Settlement-currency present value with per-curve derivatives and FX delta
`PresentValueFX::pv` SHALL discount a cashflow's (or leg's) `settlementCurrencyAmount()` using the supplied discount curve, for `FixedRateMultiCurrencyCashflow`, `IborMultiCurrencyCashflow`, `OvernightIndexMultiCurrencyCashflow`, `CompoundedOvernightRateMultiCurrencyCashflow2`, and `SimpleMultiCurrencyCashflow`, and SHALL expose, via `getNotionalCurveDerivatives()`/`getCipSettlementCurveDerivatives()`/`getDiscountCurveDerivatives()`/`getFxDelta()`, present-value derivatives with respect to every vertex of the notional curve, the CIP-projection curve (the curve previously supplied to `ForwardFXRates::setFXRateCIP` when the cashflow's forward FX rate was projected), and the discount curve, plus an FX delta. `getCipSettlementCurveDerivatives()` SHALL be computed from the cashflow's cached amount-derivatives alone (the CIP-projection curve's contribution), and `getDiscountCurveDerivatives()` SHALL be computed from the discount curve's own discount-factor derivative alone (the discount curve's contribution); neither method SHALL combine the two. The two vectors SHALL each be sized to their own curve's length independently, with no assumption that the CIP-projection curve and the discount curve share a node set, a length, or are the same object. `PresentValueFX` SHALL NOT modify the cashflow's `present_value`/`discount_factor` fields used by `record()`.

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

#### Scenario: Overnight, compounded-overnight-rate, and NDF cashflows present-value the same way
- **WHEN** `pv()` is computed for a still-floating `OvernightIndexMultiCurrencyCashflow`, `CompoundedOvernightRateMultiCurrencyCashflow2`, or `SimpleMultiCurrencyCashflow`
- **THEN** the result and all four derivative outputs follow the same discounting, per-curve-derivative, and FX-delta rules defined above for `FixedRateMultiCurrencyCashflow`/`IborMultiCurrencyCashflow`

#### Scenario: Unsupported cashflow type is rejected
- **WHEN** `pv()` is called with a cashflow type other than `FixedRateMultiCurrencyCashflow`, `IborMultiCurrencyCashflow`, `OvernightIndexMultiCurrencyCashflow`, `CompoundedOvernightRateMultiCurrencyCashflow2`, or `SimpleMultiCurrencyCashflow`
- **THEN** the call throws `std::invalid_argument`

## ADDED Requirements

### Requirement: Spot-fixing FX rate assignment covers SimpleMultiCurrencyCashflow
`ForwardFXRates::setFXRate` and `setFXRateForLeg` SHALL accept a `SimpleMultiCurrencyCashflow` (or a `Leg` containing one), reading the historical fixing for the cashflow's FX rate index fixing date from the supplied `FXRateEstimator` and setting it as the cashflow's FX rate index value, consistent with existing support for `FixedRateMultiCurrencyCashflow`, `IborMultiCurrencyCashflow`, `OvernightIndexMultiCurrencyCashflow`, and `CompoundedOvernightRateMultiCurrencyCashflow2`.

#### Scenario: Historical fixing applied to an NDF-shaped cashflow
- **WHEN** `setFXRate` is called with a `SimpleMultiCurrencyCashflow` and an `FXRateEstimator` that has a value for the cashflow's FX rate index fixing date
- **THEN** the cashflow's FX rate index value is set to that historical value
