## Context

`Operation` was introduced in `1.12.0` (change `add-operation-batch-state`) with a `std::vector<RecPay>` parallel to its legs. That change's own design document already recorded the field as "stored and echoed" — it was speculative surface for a possible future consumer (unsigned amounts plus a direction column), not a dependency of anything shipped.

The current state, verified across `include/`, `source/`, and `Tests/`:

| Site | Use |
|---|---|
| `Operation.h:30` | stored in `_recPay` |
| `Operation.h:38` | validated for length against `_legs` |
| `Operation.h:79` | returned by `getRecPay` |
| `qcf_binder.cpp:1166` | bound as `get_rec_pay` |
| `PortfolioTests.cpp:91` | asserted once |

`Portfolio::statesAt` and `Portfolio::flowsBetween` reference neither `getRecPay` nor `_recPay`; both accumulate from `cf->amount()`.

## Goals / Non-Goals

**Goals:**
- Remove `rec_pay` from `Operation`'s constructor and accessor surface.
- Keep the remaining construction validations (at least one leg; no empty leg) intact.
- Leave `Portfolio` and both batch queries byte-for-byte identical in behavior.

**Non-Goals:**
- Changing how `LegFactory` applies signs. Direction stays baked into amounts exactly as today.
- Adding a direction accessor to `Leg` or `Cashflow`. Neither stores one now and nothing needs one.
- Providing a deprecation path or overload for the three-argument constructor.
- Adding a `rec_pay` column to `StateColumns` or `FlowColumns`.

## Decisions

**Remove outright rather than deprecate.** The package is at `1.12.0a1` and `Operation` has existed for one alpha. A `[[deprecated]]` overload would carry the dead field forward through the very release where removing it is free. Alternative considered: keep the constructor parameter but drop the accessor — rejected, since that preserves the cost (every caller must supply it) while removing the only thing you could do with it.

**Do not derive direction from cashflow signs as a replacement accessor.** A convenience `Leg::direction()` returning `Receive` when amounts are positive is tempting and wrong: legs with an initial and final exchange of notional, or with a negative rate, carry both signs, so any such accessor would be a heuristic dressed as a fact. Callers that want a label keep one, in the same place they already keep the meaning of the opaque key.

**`RecPay` itself stays.** The enum lives in `cashflows/Cashflow.h` and remains the input to every `LegFactory` builder. Only `Operation`'s copy of it goes.

**The empty-legs check keeps its current error text.** Only the branch comparing `_legs.size()` to `_recPay.size()` is deleted, so the `ValueError` surfaced for a zero-leg operation is unchanged and its scenario stays green.

## Risks / Trade-offs

- **A future consumer wants per-leg direction reported in the columnar output** → It would be an additive column on `StateColumns`, computed from whatever the caller supplies at that point. Nothing about removing the field now makes that harder; `Operation.h` is header-only and the parameter is four lines to restore.
- **Out-of-repo callers break at the constructor** → Accepted and intended: the version is alpha, the failure is a compile error in C++ and an immediate `TypeError` in Python, and the fix is deleting an argument. No silent behavior change is possible, since the field fed no computation.
- **The docs notebook lives in a separate repository** (`qcfinancial-docs`) and cannot be updated atomically with this change → Sequence it: land the library change, build the wheel, then update and re-execute `8_Operaciones_Portafolio.ipynb` against it. The notebook is the only known external consumer.

## Migration Plan

1. Remove the field, validation, accessor, and binding.
2. Update `Tests/PortfolioTests.cpp` construction sites and drop the `getRecPay` assertion.
3. Rebuild the wheel and confirm the C++ tests and a Python smoke construction pass.
4. Update the docs notebook in `qcfinancial-docs` and re-execute it against the new wheel.

Rollback is a `git revert`; no data, persisted state, or serialized format is involved.

## Open Questions

- Whether `add-operation-batch-state` should be archived before this change lands, so that the `operation` delta applies against a baseline in `openspec/specs/`. That change is complete (20/20 tasks, all four artifacts) but unarchived. Archiving it first is the tidier sequence.
