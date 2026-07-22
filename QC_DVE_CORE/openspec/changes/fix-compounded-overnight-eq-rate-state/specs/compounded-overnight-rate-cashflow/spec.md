## ADDED Requirements

### Requirement: _endDateWf encodes the coupon equivalent rate

For `CompoundedOvernightRateCashflow2`, the cached wealth factor `_endDateWf` SHALL be the
wealth factor of the coupon's **equivalent rate** — the rounded (to `_eqRateDecimalPlaces`),
**bare** rate excluding gearing and spread — evaluated over the accrual period
*(startDate → endDate)*. When the coupon's fixings are set via `setFixings()` / `fixing()`,
`fixing()` SHALL store `_endDateWf` as `wf(round(eqRate, _eqRateDecimalPlaces))`, not the raw
daily-compounded product. This mirrors the wealth factor `ForwardRates` reconstructs for a
realized coupon and preserves the relation
`round(eqRate) = getRateFromWf(_endDateWf / _initialDateWf, startDate, endDate)`.

#### Scenario: fixing() stores the rounded-rate wealth factor
- **WHEN** `setFixings()` is called on a fully realized `CompoundedOvernightRateCashflow2`
- **THEN** inverting the stored `_endDateWf` via `getRateFromWf(_endDateWf, startDate, endDate)`
  yields exactly the value returned by `fixing()` (the rounded equivalent rate), not the
  unrounded compounded rate

### Requirement: amount() stays unrounded and differentiable

`amount()` SHALL compute interest by inverting `_endDateWf / _initialDateWf` with
`getRateFromWf` and applying gearing/spread via `_calculateInterest`, **without any rounding
in its own code path**, so that the projected/forward valuation regime and its curve
sensitivities (`_amountDerivatives`) remain differentiable. The rounding of a realized
coupon's rate SHALL be carried by `_endDateWf` (set upstream in `fixing()`), never introduced
inside `amount()`.

#### Scenario: forward coupon valuation unchanged
- **WHEN** a coupon is priced through `ForwardRates::setRatesCompoundedOvernightLeg2` (a
  projected or partially-accrued coupon, never routed through `setFixings()`)
- **THEN** `amount()`, its present value, and `_amountDerivatives` are identical to the
  behavior before this change

### Requirement: Realized-coupon interest is consistent across methods

Every valuation and reporting method of a fully realized `CompoundedOvernightRateCashflow2` SHALL report interest derived from the same rounded equivalent rate. A realized coupon is one whose fixings are all known and set via `setFixings()`. `amount()` net of amortization, `settlementAmount()` net of amortization, and `interest(fixings)` MUST be equal to double precision, and the `record()` cashflow, `record()` rate value, and `wrap()` interest and rate MUST reflect that same rounded equivalent rate.

#### Scenario: amount() matches settlementAmount() with spread and gearing
- **WHEN** a realized coupon is built with `spread != 0`, `gearing != 1`, and a finite
  `_eqRateDecimalPlaces` such that the equivalent rate rounds, and `setFixings()` is called
- **THEN** `amount()` minus amortization equals `settlementAmount()` minus amortization and
  equals `interest(fixings)`, all computed from `notional · ((round(eqRate) · gearing + spread) · yf)`

#### Scenario: amount() does not clobber the settlement interest
- **WHEN** `amount()` (or `record()` / `wrap()`) is called after `setFixings()`
- **THEN** a subsequent `interest(fixings)` still returns the rounded settlement interest —
  the valuation call did not leave a divergent unrounded value behind

#### Scenario: rate_value reports the rounded equivalent rate
- **WHEN** `record()` is called on a realized coupon
- **THEN** `record()["rate_value"]` equals the rounded equivalent rate returned by `fixing()`,
  and `record()["cashflow"]` equals `record()["interest"]` (plus amortization when
  `amort_is_cashflow`)
