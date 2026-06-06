## Context

`IcpClfCashflow` inherits from `IcpClpCashflow`. The parent keeps a `_interest` member that is written by `IcpClpCashflow::amount()` but never by `IcpClfCashflow::amount()` — so `_interest` is always stale in the subclass. The `record()` method was written using that stale field, and also carried a copy-paste cashflow formula and a typo. Additionally, the v1.10.1 convention (present_value + discount_factor as final two fields) was never applied.

The `wrap()` method in the same file already computes interest correctly via `accruedInterest(_endDate, _endDateICP, _endDateUF)`, and `OvernightIndexCashflow::record()` is the canonical reference for the mandatory field structure.

## Goals / Non-Goals

**Goals:**
- `record()` returns a correct `interest` value computed fresh from current ICP and UF index values
- `cashflow` field equals `interest` (no amortization) or `amortization + interest` (with amortization)
- `interest_rate_index` field reads `"ICPCLF"`
- `present_value` and `discount_factor` are the final two fields
- Output is consistent with `OvernightIndexCashflow::record()` style

**Non-Goals:**
- Changes to `wrap()`, `amount()`, or any other method
- Header or binder changes (binding already exists at `qcf_binder.cpp:980`)
- Changes to `IcpClpCashflow`

## Decisions

**Compute interest inline, same pattern as `wrap()`.**
`accruedInterest(_endDate, _endDateICP, _endDateUF)` is already the established way to get a fresh interest figure in this class. Reusing it keeps `record()` consistent with `wrap()` and avoids relying on inherited mutable state.

Alternative considered: call `amount()` and subtract `_amortization`. Rejected — `amount()` has side-effects (derivative vectors, rate mutation) and returns cashflow not pure interest.

## Risks / Trade-offs

- [Python callers using the old `record()` field values] → The fix changes field values. Since the old values were wrong (stale `_interest`, inflated cashflow), this is a correctness fix not a breaking change in spirit. The field names are unchanged.
- [No automated test for `record()`] → Verify manually after build by constructing an `IcpClfCashflow` and calling `.record()` in Python.

## Migration Plan

Single-file edit to `source/cashflows/IcpClfCashflow.cpp`. No migration required; wheel rebuild picks it up.