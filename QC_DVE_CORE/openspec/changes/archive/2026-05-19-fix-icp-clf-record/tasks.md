## 1. Fix record() in IcpClfCashflow.cpp

- [x] 1.1 Compute interest locally using `accruedInterest(_endDate, _endDateICP, _endDateUF)` instead of reading `_interest`
- [x] 1.2 Fix `cashflow` field: use `_amortization + interest` (not `_nominal + interest`) when `doesAmortize` is true
- [x] 1.3 Fix `interest_rate_index` value from `"ICPLCP"` to `"ICPCLF"`
- [x] 1.4 Add `result["present_value"] = getPresentValue()` as second-to-last field
- [x] 1.5 Add `result["discount_factor"] = getDiscountFactor()` as last field

## 2. Verify

- [x] 2.1 Build the wheel and import `qcfinancial` in Python
- [x] 2.2 Construct an `IcpClfCashflow`, call `.record()`, and confirm all five corrected fields match expected values