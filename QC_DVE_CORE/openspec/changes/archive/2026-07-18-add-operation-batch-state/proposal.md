# Proposal: add-operation-batch-state

## Why

qcalm (an agentic ALM system) projects a bank's balance sheet daily over 3+ year horizons and needs, each simulated day, the state of every leg of every live operation in a 10⁵–10⁶ operation portfolio. Today qcfinancial only exposes per-cashflow queries through per-object Python calls, so that hot path would cost O(operations) boundary crossings per day instead of O(1). The requirements are stated in `requirements/qcf_requirements_operation_state_v0.1_EN.md` (v0.1, 2026-07-17).

## What Changes

- New `Operation` type: an immutable container of one or more `Leg` objects, identified by a caller-supplied opaque integer key, with per-leg `leg_number` and `rec_pay` direction.
- New `Portfolio` container holding many Operations, built once and mutated incrementally (add on origination/renewal, remove on maturity/prepayment).
- New batch state query on the container: given date `t` (and optionally discount curves mapped by currency), return per-leg state — accrued interest, outstanding notional, optional present value, next flow date, flow settling at `t` (amount and interest/amortization split), operation key, leg number — as columnar numpy arrays (one boundary crossing per query, buffer protocol / `py::array_t`, no Arrow dependency).
- New batch flow window query: given `t1 < t2`, return all contractual flows settling in `(t1, t2]`, columnar with the same conventions (serves the RCL liquidity-ratio consumer).
- Batch queries release the GIL and parallelize across operations with plain `std::thread` chunks — **no OpenMP** (per explicit constraint; avoids libomp dependency in macOS/Linux wheels).
- Batch PV path uses thread-safe discount-factor evaluation — it must not reuse `PresentValue`/curve derivative caching, which mutates shared state on every query (per-query precomputed df table; see design).
- Uniform state surface on cashflows so the batch query dispatches without downcasts, exercised in this change only for `FixedRateCashflow` (bullet and custom amortization), but not precluding other leg types.
- Version bump in `setup.py`.

## Capabilities

### New Capabilities

- `operation`: the `Operation` type — legs, opaque key, leg numbering, direction, immutability, construction from existing `LegFactory` outputs.
- `operation-portfolio`: the `Portfolio` container — incremental add/remove by key, target scale, receiver of all batch queries.
- `portfolio-batch-queries`: the columnar batch surface — `states_at(t, curves?)` per-leg state query and `flows_between(t1, t2)` flow window, output schema, zero-per-row-conversion boundary contract, determinism, parallel execution semantics.

### Modified Capabilities

<!-- none: existing specs (business-calendar, calendar-factory, front-stub-date-schedule, icp-clf-record, overnight-index-cashflow) are untouched -->

## Impact

- **New code**: `include/portfolio/` + `source/portfolio/` (Operation, Portfolio, batch query engine); registered in `source/CMakeLists.txt`.
- **Modified code**: `Cashflow` base (uniform state accessors with safe defaults); `QcfinancialPybind11Helpers.h` + `source/qcf_binder.cpp` for bindings (introduces `pybind11/numpy.h`, already header-only in the submodule — no new dependency). Curve/interpolator classes are untouched.
- **Explicitly out**: OpenMP or any new third-party dependency; Arrow; client/product/accounting concepts; regeneration-style APIs (operations are built once); changes to existing cashflow math.
- **Consumers**: qcalm F1 (fixed-rate single-leg operations); determinism/reproducibility against pinned wheel versions is part of the contract.
