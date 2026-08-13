## Context

`PresentValueFX::pv(valuationDate, cashflow, settlementCurve)` (`source/present_value/PresentValueFX.cpp:8-62`) computes, for the settlement-curve derivative vector:

```cpp
for (size_t j = 0; j < amountSettlementCurveDerivatives.size(); ++j) {
    _settlementCurveDerivatives.at(j) = amountSettlementCurveDerivatives.at(j) * dfSettlement +
            settlementCurrencyAmount * settlementCurve->dfDerivativeAt(j);
}
```

`amountSettlementCurveDerivatives` is read off the cashflow, cached there by `settlementCurrencyAmount()` via chain rule from `_fxRateSettlementCurveDerivatives`, which `ForwardFXRates::_projectFXRateCIP` (`include/present_value/ForwardFXRates.cpp:52-88`) populated from whatever curve was passed to `setFXRateCIP` as its `settlementCurve` argument at projection time — call it **curve1**. `settlementCurve->dfDerivativeAt(j)` in the snippet above is the curve passed to `pv()` as *its* `settlementCurve` argument — call it **curve2**. Nothing ties curve1 and curve2 to be the same object; `pv()` takes curve2 fresh on every call and has no reference to whatever curve1 was.

The full PV, strong-side case, is:

```
PV = notionalAmount * Spot * DF_notional(t; curveN) / DF(t; curve1) * DF(t; curve2)
```

Three independent chain-rule paths reach `PV`: through `curveN` (untouched by this change), through `curve1` (via the forward's denominator), and through `curve2` (via the final discount factor). When `curve1 == curve2` these are the same node set, and `d(PV)/d(node_j)` genuinely requires summing both paths' contributions — which is exactly what the current code does, and it happens to be correct in that one case (the sum reduces to an exact algebraic zero on the strong side: `d/dc[A/DF(c) * DF(c)] = 0` for any functional form of `DF(c)`, matching `present-value-fx`'s design.md decision D5). When `curve1 != curve2`, there is no shared node set to sum over — the two paths are simply independent, and the current code's shared-index loop is a category error, not an approximation.

## Goals / Non-Goals

**Goals:**
- Make the two chain-rule paths (curve1-via-forward, curve2-via-discount-factor) independently retrievable, correct regardless of whether curve1 and curve2 are the same object, same length, or even the same currency.
- Preserve the existing same-curve numeric identity as a derived fact (sum of the two new vectors), not lose it — existing tests become the regression proof.
- Touch only `PresentValueFX` and its binder exposure. `ForwardFXRates`/cashflow caching already isolates curve1's contribution correctly and needs no change.

**Non-Goals:**
- No auto-summing convenience getter that re-detects "same curve" and returns a combined vector. Explicitly rejected: detecting curve identity (pointer equality? node-set equality?) is itself ambiguous, and the whole point of the split is that the caller — who knows whether they passed the same curve to both `setFXRateCIP` and `pv()` — is in a better position to decide than the library.
- No change to `getFxDelta()` or `getNotionalCurveDerivatives()` — neither involves curve2 at all (`_fxDelta = amountFxDelta * dfSettlement` is a plain scalar product with a value, not a per-vertex derivative; `dfSettlement` doesn't depend on spot).
- No change to `ForwardFXRates::setFXRateCIP`'s two-curve signature — it already correctly takes exactly the one curve (curve1) relevant to projection.
- Not extending to `OvernightIndexMultiCurrencyCashflow`/`CompoundedOvernightRateMultiCurrencyCashflow2` — same deferral as the original `present-value-fx` change.

## Decisions

**D1 — Two independent vectors, not one vector plus a cross-term.**
Considered keeping one vector sized to `max(curve1.length, curve2.length)` with a documented "only valid if curve1==curve2" caveat. Rejected: it doesn't fix the out-of-bounds/misalignment failure mode, just documents it — a caller who ignores the caveat still gets garbage, and there's no way to recover curve1's and curve2's individual sensitivities from the combined vector once summed. Two vectors, one per curve, each always dimensioned to its own curve's length, has no failure mode to document.

**D2 — Rename, don't keep the old getter as a deprecated alias.**
`getSettlementCurveDerivatives()` is removed outright rather than kept as a sum-of-the-two-new-vectors convenience method. This is the same reasoning as the Non-Goals entry above: a same-named method that used to mean "the settlement curve's total derivative" and now silently means "assume the two curves coincide and sum" is a worse trap than a compile error at every call site. Since `present-value-fx` has not shipped past `1.14.0a1`, there is no external consumer to cushion.

**D3 — `pv()`'s curve parameter renamed `settlementCurve` → `discountCurve`.**
Header-only rename (`include/present_value/PresentValueFX.h`); the pybind11 binding at `source/qcf_binder.cpp:1951-1960` binds `pv` positionally (no `py::arg` names), so this is not a Python-facing break. Motivation: "settlementCurve" was always a slight misnomer for this parameter (it's the curve `pv()` discounts with — it has no inherent tie to "the settlement currency's curve" once curve1/curve2 can differ), and the rename removes the last piece of naming that implied curve1 and curve2 were the same thing.

**D4 — Which term is "CIP" and which is "discount" follows the existing cached-derivative provenance, not a new parameter.**
`getCipSettlementCurveDerivatives()` returns exactly `amountSettlementCurveDerivatives.at(j) * dfDiscount` (curve1's path, already fully computed and cached on the cashflow — no new computation, just no longer summed away). `getDiscountCurveDerivatives()` returns exactly `settlementCurrencyAmount * discountCurve->dfDerivativeAt(j)` (curve2's path). `PresentValueFX` does not need to know curve1's identity or length beyond what's already implicit in the cached vector's size — it never receives curve1 as a parameter, matching today's signature.

## Risks / Trade-offs

- **[Risk] A caller who never split their curves (curve1 == curve2, the common case) forgets to sum the two new vectors and treats one alone as "the" settlement-curve sensitivity.** → Mitigation: no old-named getter survives to be called by habit; the caller must consciously call two new, differently-named methods, and existing test rewrites double as the worked example of the correct sum.
- **[Risk] `getCipSettlementCurveDerivatives()`'s name presumes the caller knows which curve produced the cached forward-FX derivatives — if a leg's cashflows were projected with different curve1s per-cashflow (unusual but not prevented by the API), the leg-level `pv(leg, ...)` overload sums per-cashflow vectors that may not share a node set.** → Mitigation: same pre-existing constraint as `getNotionalCurveDerivatives()` at leg level today (leg-level summation already assumes one notional curve for the whole leg); out of scope for this change, not a regression it introduces.
- **[Trade-off] Two getters instead of one is a slightly larger surface.** Accepted — it's the direct cost of removing an incorrect implicit assumption, and matches the user's explicit design decision (three independent vectors: notional, CIP-settlement, discount).

## Migration Plan

Pre-release breaking change within the still-unshipped `1.14.0` line (`setup.py` currently `1.14.0a1`). No data migration. Steps:
1. Implement the split in `PresentValueFX.h`/`.cpp`.
2. Update `source/qcf_binder.cpp` bindings and `id()` version string.
3. Rewrite the 4 affected `REQUIRE`s in `Tests/PresentValueFXTests.cpp` to sum the two new getters; add new tests for distinct curve1/curve2 (including mismatched lengths).
4. Bump version to `1.14.0a2` in `setup.py`, `CLAUDE.md`, `qcf_binder.cpp`.
5. Commit per this repo's `# Update to Version 1.14.0a2: <description>` convention.

Rollback: plain revert, no persisted state involved.

## Open Questions

- None outstanding. Whether/when to run `openspec archive` on the pending `present-value-fx` change (its spec delta hasn't been merged into `openspec/specs/` yet) is orthogonal to this change and left to the user.
