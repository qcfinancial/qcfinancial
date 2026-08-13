## ADDED Requirements

### Requirement: CIP forward FX projection onto multi-currency cashflows
`ForwardFXRates::setFXRateCIP` SHALL project a forward FX rate for a `FixedRateMultiCurrencyCashflow` or `IborMultiCurrencyCashflow` as `Forward(t) = Spot * DF_notional(t) / DF_settlement(t)`, where `t` is the valuation date's day difference to the cashflow's end date, `DF_notional`/`DF_settlement` are discount factors from the supplied notional-currency and settlement-currency curves, and SHALL cache the forward's derivative with respect to every vertex of both curves on the cashflow. `ForwardFXRates::setFXRateForLegCIP` SHALL apply this to every cashflow in a `Leg`. Cashflow types other than the two listed SHALL cause `std::invalid_argument`.

#### Scenario: Forward FX projected for a still-floating cashflow
- **WHEN** `setFXRateCIP` is called with a valuation date on or before the cashflow's FX fixing date
- **THEN** the cashflow's FX rate index value is set to `Spot * DF_notional(t) / DF_settlement(t)`, and its notional-curve and settlement-curve FX-forward derivative vectors are populated (non-zero for at least one vertex when the corresponding curve is not flat at that vertex)

#### Scenario: Already-fixed cashflow is left untouched
- **WHEN** `setFXRateCIP` is called with a valuation date after the cashflow's FX fixing date
- **THEN** the cashflow's stored FX rate index value is unchanged, and both FX-forward derivative vectors are set to all zeros

#### Scenario: Unsupported cashflow type is rejected
- **WHEN** `setFXRateCIP` is called with a multi-currency cashflow type other than `FixedRateMultiCurrencyCashflow` or `IborMultiCurrencyCashflow`
- **THEN** the call throws `std::invalid_argument`

### Requirement: Curve-vertex amount derivatives cached without rounding
`FixedRateMultiCurrencyCashflow::settlementCurrencyAmount()` and `IborMultiCurrencyCashflow::settlementCurrencyAmount()` SHALL, on every call, cache curve-vertex derivatives of the returned (unrounded) settlement-currency amount with respect to both the notional and settlement curves, and a derivative with respect to the spot FX value, derived by chain rule from the cashflow's cached FX-forward derivatives. The rounding method `settlementAmount()` SHALL NOT be used as the basis for any derivative computation.

#### Scenario: Strong-side notional currency
- **WHEN** the cashflow's notional currency is the strong side of its `FXRateIndex` pair and its FX-forward derivatives are non-zero
- **THEN** `settlementCurrencyAmount()`'s cached amount-derivatives equal the notional amount times the corresponding cached FX-forward derivative, and the cached FX-spot amount-derivative equals the settlement-currency amount divided by the spot value

#### Scenario: Weak-side notional currency
- **WHEN** the cashflow's notional currency is the weak side of its `FXRateIndex` pair and its FX-forward derivatives are non-zero
- **THEN** `settlementCurrencyAmount()`'s cached amount-derivatives and cached FX-spot amount-derivative carry the opposite sign and the inverse-square scaling implied by `settlementCurrencyAmount = notionalAmount / Forward`

#### Scenario: Already-fixed cashflow yields zero amount-derivatives
- **WHEN** `settlementCurrencyAmount()` is called on a cashflow whose FX-forward derivatives were zeroed by the already-fixed projection path
- **THEN** the cached amount-derivatives with respect to both curves and to spot are all zero

### Requirement: Settlement-currency present value with per-curve derivatives and FX delta
`PresentValueFX::pv` SHALL discount a cashflow's (or leg's) `settlementCurrencyAmount()` using the supplied settlement curve, and SHALL expose, via `getNotionalCurveDerivatives()`/`getSettlementCurveDerivatives()`/`getFxDelta()`, present-value derivatives with respect to every vertex of the notional and settlement curves and an FX delta, computed as a product-rule combination of the cashflow's cached amount-derivatives with the settlement curve's own discount-factor derivative. `PresentValueFX` SHALL NOT modify the cashflow's `present_value`/`discount_factor` fields used by `record()`.

#### Scenario: Matured cashflow contributes zero
- **WHEN** the valuation date is on or after the cashflow's end date
- **THEN** `pv()` returns `0.0` and all derivative vectors and the FX delta are zero

#### Scenario: Strong-side settlement curve derivative cancels to zero
- **WHEN** `pv()` is computed for a still-floating, strong-side-notional cashflow using the same two curves that produced its cached FX-forward derivatives
- **THEN** the settlement-curve present-value derivative is zero at every vertex, and `pv()` equals the notional amount times the spot value times the notional curve's discount factor at the cashflow's end date

#### Scenario: Weak-side settlement curve derivative is non-zero
- **WHEN** `pv()` is computed for a still-floating, weak-side-notional cashflow
- **THEN** the settlement-curve present-value derivative is non-zero for at least one vertex where the settlement curve's own discount-factor derivative is non-zero

#### Scenario: FX delta sign follows strong/weak side
- **WHEN** `pv()` is computed for a still-floating cashflow
- **THEN** the FX delta equals `+pv()/Spot` when the notional currency is the strong side, and `-pv()/Spot` when it is the weak side

#### Scenario: Already-fixed cashflow has zero FX delta
- **WHEN** `pv()` is computed for a cashflow whose FX rate has already fixed
- **THEN** the FX delta is zero and the notional-curve present-value derivative is zero at every vertex, while the settlement-curve present-value derivative is non-zero (ordinary discounting of a fixed amount)

#### Scenario: Leg-level FX delta is a per-cashflow sum
- **WHEN** `pv()` is computed for a `Leg` containing at least one already-fixed and at least one still-floating cashflow
- **THEN** the leg's FX delta equals the sum of the FX deltas each cashflow would produce individually, and is not equal to `pv() / Spot`

#### Scenario: Unsupported cashflow type is rejected
- **WHEN** `pv()` is called with a cashflow type other than `FixedRateMultiCurrencyCashflow` or `IborMultiCurrencyCashflow`
- **THEN** the call throws `std::invalid_argument`
