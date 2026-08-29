## Context

See proposal.md - Why. Relevant current-state facts that shape the approach:

- `FixedRateMultiCurrencyCashflow`/`IborMultiCurrencyCashflow` already implement the full pattern this change replicates: 4 setters (`setFxRateNotionalCurveDerivatives`, `setFxRateSettlementCurveDerivatives`, `setFxRateSpotDerivative`, `setFxRateIndexValue`) + 3 getters (`getAmountNotionalCurveDerivatives`, `getAmountSettlementCurveDerivatives`, `getAmountFxDelta`) + a `settlementCurrencyAmount()` that caches the strong/weak-side chain-rule derivatives (`source/cashflows/FixedRateMultiCurrencyCashflow.cpp:219-253`).
- `ForwardFXRates::_projectFXRateCIP<T>` (`source/present_value/ForwardFXRates.cpp:52-88`) is already a template requiring only `setFxRateIndexValue`, the three setters above, and `.endDate()` on `T` — it needs no changes, only new call sites.
- `PresentValueFX::pv` (`source/present_value/PresentValueFX.cpp:8-69`) dispatches on `cashflow->getType()` string and calls the same five-method surface — again, only new branches, no change to its logic.
- `OvernightIndexMultiCurrencyCashflow` and `CompoundedOvernightRateMultiCurrencyCashflow2` already have `settlementCurrencyAmount()` and are already wired into `ForwardFXRates::setFXRate` (spot-fixing). They're missing the derivative-caching surface and the CIP/PresentValueFX call sites.
- `SimpleMultiCurrencyCashflow` has none of this: no derivative-caching surface, no `settlementCurrencyAmount()` (its `settlementAmount()` does the FX conversion inline, unrounded, with no caching), and is entirely absent from `ForwardFXRates::setFXRate`.
- `IcpClfCashflow` is excluded (see proposal.md - What Changes): different mechanism entirely (ICP/UF curves, no `FXRateIndex`, hardcoded CLP settlement, its own derivative caching already inside `amount()`).

## Goals / Non-Goals

**Goals:**
- Bring `OvernightIndexMultiCurrencyCashflow`, `CompoundedOvernightRateMultiCurrencyCashflow2`, and `SimpleMultiCurrencyCashflow` to full parity with `FixedRateMultiCurrencyCashflow`/`IborMultiCurrencyCashflow` on the CIP-forward-projection and `PresentValueFX` surface.
- Preserve `SimpleMultiCurrencyCashflow::settlementAmount()`'s existing numeric behavior exactly while adding derivative caching.
- Produce marimo + Excel artifacts a human reviewer can use to independently sanity-check the closed-form math for all five multi-currency types, without needing to run the C++ test suite.

**Non-Goals:**
- `IcpClfCashflow` settlement-currency support (explicitly deferred/out of scope).
- A `LegFactory` builder for `SimpleMultiCurrencyCashflow` (single-cashflow / manually-assembled-`Leg` API is sufficient for NDF usage).
- Finite-difference cross-validation in the Excel workbook (closed-form only, per prior decision).
- Any change to `Portfolio`/`statesAt`/`flowsBetween` — this change is scoped to the cashflow/PresentValueFX layer, not the batch-query layer.

## Decisions

**Copy the existing pattern verbatim rather than introduce a shared base/mixin.** `FixedRateMultiCurrencyCashflow` and `IborMultiCurrencyCashflow` already duplicate this derivative-caching logic independently (no shared base between them beyond `Cashflow`), and `ForwardFXRates`/`PresentValueFX` already dispatch via `getType()` string rather than a common interface. Introducing a mixin now would mean refactoring two already-shipped, tested classes purely for this change's benefit — out of proportion to the ask. Follow the codebase's existing convention (duplication with a template only where it already existed, i.e., `_projectFXRateCIP<T>`).

**`SimpleMultiCurrencyCashflow::settlementCurrencyAmount()` becomes the new home for the conversion math; `settlementAmount()` calls it.** Today `settlementAmount()` does the strong/weak-side division/multiplication directly with no caching. Moving that logic into `settlementCurrencyAmount()` (matching the naming and call-direction convention of the other four types, where the public rounding/settlement method calls the derivative-caching method — see `OvernightIndexMultiCurrencyCashflow.cpp:164`) and having `settlementAmount()` call it keeps `settlementAmount()`'s return value byte-identical (no rounding is added, since none existed before) while giving `PresentValueFX` a caching method to call.

**No new `LegFactory` builder for NDFs.** Confirmed with the user: NDFs are constructed one `SimpleMultiCurrencyCashflow` at a time in Python, not from a schedule. `Leg`-level calls (`setFXRateForLeg`, `setFXRateForLegCIP`, `PresentValueFX::pv(Leg&, ...)`) already iterate any manually-assembled `Leg`, so no factory method is required for Leg-level support to work.

**Excel workbook uses closed-form formulas only, cross-checked visually against the spec, not finite-difference bumping.** Confirmed with the user. Trade-off: a bug shared between the C++ chain-rule derivation and the Excel formula's derivation would not be caught by this artifact — see Risks below.

**`OvernightIndexMultiCurrencyCashflow::settlementCurrencyAmount()` is fixed to be unrounded, discovered during implementation.** Unlike the other four types, its pre-existing implementation computed `settlementCurrencyInterest()` + `settlementCurrencyAmortization()`, each of which rounds twice (`ccy()->amount(...)` in notional currency, then `_settlementCurrency->amount(...)` after conversion) — making a curve-vertex/spot derivative of the result mathematically undefined, in direct conflict with the "cached without rounding" requirement this change extends. User-confirmed fix (2026-08-29): `settlementCurrencyAmount()` now recomputes the notional-currency interest/amortization from `_calculateInterest`/`_amortization` directly and converts once, unrounded — matching the pattern of the other four types. `settlementAmount()` is unchanged (still rounds once, via `_settlementCurrency->amount(settlementCurrencyAmount())`), but its numeric output shifts at the sub-cent level since it previously sat on top of a value that had already been rounded twice. `settlementCurrencyInterest()`/`settlementCurrencyAmortization()` themselves are untouched — their existing rounded contract (used by `record()`/`mccyWrap()`) is a separate, pre-existing concern out of scope here.

**Python review environment uses `uv` for an in-project Python 3.14 venv, deferred to the apply phase.** This is tooling scaffolding with no bearing on the specs above; sequencing it after the C++/binder work means the notebook can import the freshly-built `qcfinancial` wheel rather than being built and left unused if the C++ work stalls.

## Risks / Trade-offs

- [Excel closed-form-only verification can't catch a shared derivation bug between C++ and Excel] → Mitigation: the marimo notebook calls the actual compiled `qcfinancial` extension and can be diffed against the Excel workbook's output cell-by-cell for the same inputs; a discrepancy between the two independently-authored artifacts (Python calling C++ vs. hand-written Excel formulas) is still a meaningful cross-check even without finite-difference bumping.
- [`SimpleMultiCurrencyCashflow` refactor touches a method (`settlementAmount()`) that may have external callers in Python notebooks/scripts outside this repo] → Mitigation: the refactor is designed to be strictly behavior-preserving (same formula, same inputs, no rounding change); add a regression test asserting `settlementAmount()`'s output is unchanged for a fixed set of inputs before and after.
- [Widening `PresentValueFX::pv`'s and `setFXRateCIP`'s supported-type dispatch is a string comparison on `getType()` — a typo in a new branch's string literal fails silently into the `std::invalid_argument` fallthrough rather than a compile error] → Mitigation: each new branch's scenario test in `PresentValueFXTests.cpp` exercises the real type end-to-end, so a mismatched string surfaces immediately as a thrown exception in the test, not silently.

## Migration Plan

- Purely additive at the API level; no existing call site needs to change.
- Version bump per project convention (`setup.py`, `CLAUDE.md` "Current version", `id()` string in `source/qcf_binder.cpp`) — proposed `1.14.0a3`.
- No data migration; no rollback complexity beyond a normal revert (new methods/branches only, one refactor with preserved output).
