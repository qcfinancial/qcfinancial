# Design: fix-overnight-index-eq-rate-state

## Context

`OvernightIndexCashflow` derives an equivalent rate from two index fixings (`_startDateIndexValue`, `_endDateIndexValue`), rounds it to `_eqRateDecimalPlaces`, applies `_gearing`/`_spread`, and converts to interest via `_rate.wf()` over the accrual period. The eq-rate logic currently exists in three copies:

1. `getEqRate(date, indexValue)` — canonical: guards `date <= _startDate` and `yf == 0` (returns 0), skips rounding when `_eqRateDecimalPlaces > 12` (`LIMIT_EQ_RATE_DECIMAL_PLACES`).
2. `settlementAmount()` — inline reimplementation: no guards, always rounds (even at 20 decimal places, where `round(x·1e20)/1e20` is float noise).
3. `amount()` — calls `getEqRate` but first mutates `_eqRateDecimalPlaces = 20` as a "don't round" sentinel, restoring it only on normal exits; it also drops `_gearing`.

Additionally `record()` and the multi-currency subclass call `_calculateInterest(_endDate, …)` unconditionally, which in `qcIndex` mode computes the eq-rate over the mismatched period *(indexStart → accrualEnd)* while `settlementAmount()`/`getRateValue()`/`wrap()` correctly use *(indexStart → indexEnd)*.

Constraints: the Python API surface (bindings in `qcf_binder.cpp`) must not change; `OvernightIndexMultiCurrencyCashflow` inherits protected state and `_calculateInterest`, so fixes must land where both classes route through.

## Goals / Non-Goals

**Goals:**
- One source of truth for the eq-rate; every valuation/reporting path honors the same state (`_gearing`, `_spread`, `_datesForEquivalentRate`, `_eqRateDecimalPlaces`, guards).
- No persistent member mutation of `_eqRateDecimalPlaces` during `amount()`.
- Regression tests for `gearing != 1` and `qcIndex` with index dates != accrual dates.

**Non-Goals:**
- Making valuation methods fully `const` / removing `_rate.setValue()` mutation (thread-safety of shared legs) — larger refactor, deferred.
- Changing `wrap()`'s interest-from-currency-rounded-cashflow reporting convention.
- Adding an "index has been fixed" flag (defaults of 1.0 stay as-is).
- Touching the deprecated cashflow types.

## Decisions

**D1 — Consolidate via a private helper `_eqRateAt(QCDate& date, double indexValue, unsigned int decimalPlaces)` (distinct name, NOT an overload of `getEqRate`).**
Private, not protected: `OvernightIndexMultiCurrencyCashflow` never touches eq-rate logic directly (it routes through protected `_calculateInterest()` / `_finalInterest()`), so the subclass needs no access. Note this adds the first `private:` section to a header that currently only has public/protected.
The existing public `getEqRate(date, indexValue)` becomes a one-line forward passing `_eqRateDecimalPlaces`. All guards (`date <= _startDate` → 0, `yf == 0` → 0, `decimalPlaces > 12` → no rounding) live only in the helper. A distinct name is required because `qcf_binder.cpp:917` binds `get_eq_rate` via a plain member-function pointer (`&OvernightIndexCashflow::getEqRate`); adding any overload — even protected, since access is checked after overload resolution — makes that expression ambiguous and breaks the binder compile. Alternative considered: an RAII guard restoring `_eqRateDecimalPlaces` — rejected; it patches the symptom (exception safety) while keeping the sentinel hack and member mutation.

**D2 — `amount()` requests no rounding by passing `decimalPlaces > LIMIT` (e.g. `LIMIT_EQ_RATE_DECIMAL_PLACES + 1`) to the overload.**
Preserves current intended behavior (PV uses the unrounded rate) without touching the member. The save/restore of `_eqRateDecimalPlaces` is deleted.

**D3 — `amount()` applies gearing: `_rate.setValue(eqRate * _gearing + _spread)`.**
Matches `settlementAmount()`, `_calculateInterest()`, and the 3-arg `accruedInterest()`. This changes PV results for `gearing != 1` — that is the fix.

**D4 — Introduce a private `_endDateForEqRate()` helper returning `_endDate` or `_indexEndDate` per `_datesForEquivalentRate`, and a protected `_finalInterest()` (or equivalent) used by `record()`, `wrap()`, and the subclass.**
`record()` and `OvernightIndexMultiCurrencyCashflow::{settlementCurrencyInterest, mccyWrap}` currently hardcode `_endDate` for the eq-rate date. Routing them through one helper fixes all three call sites at the shared level rather than per caller. The eq-rate is evaluated at `_endDateForEqRate()`; the wealth factor is always applied over the accrual period `(_startDate, _endDate)` — unchanged.

**D5 — `settlementAmount()` drops its inline eq-rate math and calls the consolidated overload with `_eqRateDecimalPlaces`.**
It gains the zero-`yf` and `date <= start` guards (returns amortization-only / zero interest instead of NaN) and the `> 12` no-rounding semantics for free. Existing results are unchanged for the normal case because the math is identical.

**D6 — The `_datesForEquivalentRate == qcAccrual` vs `qcIndex` branch for the yf start date stays inside the consolidated `getEqRate`; callers only choose the *end* date.** This keeps the existing public behavior of `getEqRate(date, value)` intact for Python users who call it directly.

**D7 — 1-arg `accruedInterest` honors `qcIndex` at period end only.**
When the passed accrual date is `>= _endDate`, the eq-rate date maps to `_endDateForEqRate()` (so the passed index value is interpreted as the fixing at the index end date, which is what callers have at that point); the TimeSeries overload correspondingly looks up the fixing at the eq-rate date instead of the accrual date. Strictly mid-period the single-date contract stands. Rationale: the cashflow stores only endpoint index dates — no calendar or lag rule — so a mid-period accrual date cannot be mapped to its index date; the 3-arg overload exists for that. Alternative considered: storing a date-lag rule to derive index dates for any accrual date — rejected as new state and constructor surface for a case the 3-arg overload already serves. Binder docstrings for all three `accrued_interest` overloads must be updated to state this.

## Risks / Trade-offs

- [Numbers change for live users with `gearing != 1` or `qcIndex` + lagged index dates] → These are bug fixes; call them out explicitly in the release notes / commit message. The affected combinations were internally inconsistent before (PV vs settlement disagreed), so no correct downstream result existed to preserve.
- [`settlementAmount()` behavior change when `_eqRateDecimalPlaces > 12`] → Previously it "rounded" at 1e13+; now it skips rounding like `getEqRate`. Practically invisible (differences are below double precision noise at 13+ places), and it makes the class self-consistent.
- [Hidden callers relying on `_rate`'s post-`amount()` value (unrounded, ungeared)] → Grep shows `getRateValue()`/`getTypeOfRate()` recompute or don't read the value; bindings expose no raw `_rate` value getter for this class. Low risk.
- [Subclass drift] → `OvernightIndexMultiCurrencyCashflow` must be changed in the same commit; its `settlementCurrencyInterest()` is the multi-currency analogue of bug #2.

## Migration Plan

Single library change; no data or API migration. Version bump in `setup.py` (1.11.3 → 1.11.4), commit message `# Update to Version 1.11.4: …` per convention. Rollback = revert the commit.

## Open Questions

- None blocking. If the C++ Catch2 test target remains commented out in `Tests/CMakeLists.txt`, regression tests can be exercised via the Python bindings instead (see tasks).
