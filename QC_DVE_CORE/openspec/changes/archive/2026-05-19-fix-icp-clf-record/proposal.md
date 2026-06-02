## Why

`IcpClfCashflow::record()` exists and is exposed to Python, but contains four bugs that make it return wrong or incomplete data. Any code building DataFrames from IcpClf legs via `record()` gets stale interest values, an incorrect cashflow amount, a misspelled index name, and is missing the mandatory `present_value` / `discount_factor` fields that all other cashflow types supply.

## What Changes

- Fix `record()` to compute interest using `accruedInterest(_endDate, _endDateICP, _endDateUF)` instead of the stale `_interest` parent field
- Fix cashflow field: use `_amortization + interest` (not `_nominal + interest`) when `doesAmortize` is true
- Fix typo `"ICPLCP"` → `"ICPCLF"` in the `interest_rate_index` field
- Add mandatory `present_value` and `discount_factor` fields at the end of the record

## Capabilities

### New Capabilities

- `icp-clf-record`: Correct, complete `record()` output for `IcpClfCashflow`, consistent with all other cashflow types

### Modified Capabilities

<!-- none -->

## Impact

- `source/cashflows/IcpClfCashflow.cpp` — `record()` implementation only
- Python consumers of `IcpClfCashflow.record()` will see corrected field values and two new fields (`present_value`, `discount_factor`)
- No header changes, no binder changes, no ABI impact