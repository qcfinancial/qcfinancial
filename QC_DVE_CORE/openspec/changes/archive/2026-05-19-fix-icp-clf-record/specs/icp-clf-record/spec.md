## Record Output Contract

`IcpClfCashflow::record()` SHALL return a map with exactly the following 21 fields, in this order:

| # | Key | Type | Value source | Notes |
|---|-----|------|-------------|-------|
| 1 | `type_of_cashflow` | string | `"icpclf"` | |
| 2 | `start_date` | string | `_startDate.description(false)` | |
| 3 | `end_date` | string | `_endDate.description(false)` | |
| 4 | `settlement_date` | string | `_settlementDate.description(false)` | |
| 5 | `notional` | double | `_nominal` | |
| 6 | `amortization` | double | `_amortization` | |
| 7 | `interest` | double | `accruedInterest(_endDate, _endDateICP, _endDateUF)` | computed fresh |
| 8 | `amort_is_cashflow` | bool | `_doesAmortize` | |
| 9 | `cashflow` | double | `interest` or `_amortization + interest` | depends on `doesAmortize` |
| 10 | `notional_currency` | string | `"CLF"` | |
| 11 | `interest_rate_index` | string | `"ICPCLF"` | |
| 12 | `start_date_index` | double | `_startDateICP` | |
| 13 | `end_date_index` | double | `_endDateICP` | |
| 14 | `start_date_uf` | double | `_startDateUF` | IcpClf-specific |
| 15 | `end_date_uf` | double | `_endDateUF` | IcpClf-specific |
| 16 | `rate_value` | double | `_rate.getValue()` | TRA |
| 17 | `spread` | double | `_spread` | |
| 18 | `gearing` | double | `_gearing` | |
| 19 | `type_of_rate` | string | `getTypeOfRate()` | e.g. `"LinAct360"` |
| 20 | `present_value` | double | `getPresentValue()` | mandatory, second-to-last |
| 21 | `discount_factor` | double | `getDiscountFactor()` | mandatory, last |

---

## ADDED Requirements

### Requirement: record() returns correct interest
`IcpClfCashflow::record()` SHALL compute interest fresh from current index values using `accruedInterest(_endDate, _endDateICP, _endDateUF)`. It MUST NOT read the inherited `_interest` field.

#### Scenario: Interest matches accruedInterest
- **WHEN** `record()` is called on an `IcpClfCashflow` with known ICP and UF values
- **THEN** `record()["interest"]` equals `accruedInterest(endDate, endDateICP, endDateUF)`

### Requirement: record() returns correct cashflow
`IcpClfCashflow::record()` SHALL set `cashflow` to `interest` when `doesAmortize` is false, and to `amortization + interest` when `doesAmortize` is true.

#### Scenario: cashflow without amortization
- **WHEN** `doesAmortize` is false
- **THEN** `record()["cashflow"]` equals `record()["interest"]`

#### Scenario: cashflow with amortization
- **WHEN** `doesAmortize` is true
- **THEN** `record()["cashflow"]` equals `record()["amortization"] + record()["interest"]`

### Requirement: record() uses correct interest rate index name
`IcpClfCashflow::record()` SHALL set `interest_rate_index` to `"ICPCLF"`.

#### Scenario: index name is ICPCLF
- **WHEN** `record()` is called
- **THEN** `record()["interest_rate_index"]` equals `"ICPCLF"`

### Requirement: record() includes present_value and discount_factor
`IcpClfCashflow::record()` SHALL include `present_value` and `discount_factor` as its final two fields, consistent with all other cashflow types.

#### Scenario: mandatory pricing fields present
- **WHEN** `record()` is called
- **THEN** `record()["present_value"]` and `record()["discount_factor"]` are both present in the returned map