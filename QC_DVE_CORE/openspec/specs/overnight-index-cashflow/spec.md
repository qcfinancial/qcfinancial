# overnight-index-cashflow

Equivalent-rate computation semantics for `OvernightIndexCashflow` and its multi-currency subclass (`OvernightIndexMultiCurrencyCashflow`). Established in change `fix-overnight-index-eq-rate-state` (v1.11.4).

### Requirement: Single equivalent-rate computation path
`OvernightIndexCashflow` SHALL compute the equivalent rate in exactly one code path, parameterized by evaluation date, index value, and decimal places. All valuation and reporting methods (`amount()`, `settlementAmount()`, `record()`, `wrap()`, `getRateValue()`, `accruedInterest()`, `_calculateInterest()`) MUST route through it. The path SHALL apply these rules: return 0 when the evaluation date is on or before the eq-rate start date; return 0 when the year fraction is 0; skip rounding when decimal places exceed 12; otherwise round half away from zero to the given decimal places.

#### Scenario: Degenerate period in settlementAmount
- **WHEN** `settlementAmount()` is called on a cashflow whose eq-rate period has zero year fraction (equal start/end dates)
- **THEN** the interest component is 0 (no NaN/inf), and the result is the amortization if `_doesAmortize`, else 0

#### Scenario: Decimal places above the rounding limit
- **WHEN** `_eqRateDecimalPlaces` is set above 12 and `settlementAmount()` or `wrap()` is called
- **THEN** the equivalent rate is used unrounded, identically in both methods

### Requirement: Gearing applied uniformly
Every method that converts the equivalent rate to interest SHALL set the rate value as `eqRate * gearing + spread`. In particular `amount()` MUST produce the same interest as `settlementAmount()` (before currency rounding) for identical state.

#### Scenario: amount() with gearing different from 1
- **WHEN** a cashflow is constructed with `gearing = 2.0`, `spread = 0.001`, fixed start/end index values, and `amount()` is called
- **THEN** the interest equals `notional * (wf(start, end; 2.0 * eqRate + 0.001) - 1)` and matches `settlementAmount()` minus amortization, up to currency and eq-rate rounding

### Requirement: Equivalent-rate end date follows DatesForEquivalentRate
When `_datesForEquivalentRate == qcIndex`, all methods computing final interest (`record()`, `wrap()`, `settlementAmount()`, and `OvernightIndexMultiCurrencyCashflow::settlementCurrencyInterest()` / `mccyWrap()`) SHALL evaluate the equivalent rate over the index period *(indexStartDate → indexEndDate)*; when `qcAccrual`, over the accrual period. The wealth factor for interest SHALL always be applied over the accrual period *(startDate → endDate)*.

#### Scenario: record() in qcIndex mode with lagged index dates
- **WHEN** a cashflow has `datesForEquivalentRate = qcIndex` and index dates shifted relative to accrual dates, and `record()` is called
- **THEN** the reported `interest` is computed from the same equivalent rate as the reported `rate_value` (eq-rate over the index period), and `cashflow` equals `interest + amortization` when `amort_is_cashflow`

#### Scenario: Multi-currency interest in qcIndex mode
- **WHEN** `OvernightIndexMultiCurrencyCashflow::settlementCurrencyInterest()` is called in `qcIndex` mode with lagged index dates
- **THEN** the notional-currency interest uses the eq-rate over the index period, consistent with the base class `settlementAmount()`

### Requirement: Interest is consistent across query methods
For both values of `DatesForEquivalentRate`, the final interest SHALL be identical whether obtained via any `accruedInterest` overload called at the accrual end date, via `record()["interest"]`, or via `settlementAmount()` minus amortization — exact match among `accruedInterest` and `record()`, up to notional-currency rounding for `settlementAmount()` and `wrap()`. To make this hold in `qcIndex` mode, the 1-arg `accruedInterest(date, indexValue)` and the TimeSeries overload SHALL, when `date >= accrualEndDate`, evaluate the eq-rate at the index end date (the passed/looked-up index value is then the fixing at the index end date; the TimeSeries overload SHALL look up the fixing at the eq-rate date). For dates strictly inside the accrual period, the 1-arg and TimeSeries overloads retain their documented single-date behavior (date used as both accrual and index date); the 3-arg overload is the precise tool mid-period in `qcIndex` mode.

#### Scenario: Consistency matrix in qcAccrual mode
- **WHEN** a cashflow with `qcAccrual`, fixed index values, and nonzero spread/gearing is queried via 1-arg `accruedInterest` at end date, 3-arg `accruedInterest`, `record()`, `wrap()`, and `settlementAmount()`
- **THEN** all report the same interest (exact for accruedInterest/record, up to currency rounding for wrap/settlementAmount)

#### Scenario: Consistency matrix in qcIndex mode with lagged index dates
- **WHEN** the same cashflow is configured with `qcIndex` and index dates shifted from accrual dates, and is queried at the accrual end date via 1-arg `accruedInterest(endDate, endIndexValue)`, 3-arg `accruedInterest(endDate, indexEndDate, endIndexValue)`, the TimeSeries overload (fixing stored at the index end date), `record()`, `wrap()`, and `settlementAmount()`
- **THEN** all report the same interest (same tolerances)

#### Scenario: Mid-period single-date contract unchanged in qcIndex mode
- **WHEN** the 1-arg `accruedInterest(d, indexValue)` is called with `d` strictly between the accrual start and end dates in `qcIndex` mode
- **THEN** the eq-rate is evaluated at `d` (single-date contract), matching the 3-arg overload called as `accruedInterest(d, d, indexValue)`

### Requirement: Valuation does not corrupt persistent state
`amount()` SHALL NOT persistently modify `_eqRateDecimalPlaces` (or any other configuration member), including when an exception propagates out of the valuation. The "unrounded rate for PV" behavior SHALL be expressed by parameter, not by mutating and restoring the member.

#### Scenario: Decimal places unchanged after amount()
- **WHEN** a cashflow with `_eqRateDecimalPlaces = 8` has `amount()` called (successfully or not)
- **THEN** `getEqRateDecimalPlaces()` returns 8, and a subsequent `wrap()`/`record()` rounds the equivalent rate to 8 places
