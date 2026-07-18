# Requirements for `qcfinancial`: `Operation` and Batch State Calculation v0.1

**Date:** 2026-07-17
**Origin:** qcalm — an agentic ALM system whose core is a daily balance-sheet projection engine. This document states *what* qcalm needs from `qcfinancial`; the *how* belongs to the qcfinancial codebase and its openspec workflow. Companion documents (in the qcalm repo): `alm_agentic_system_design_v0.5_EN.md`, `alm_architecture_f1_v0.1_EN.md`.

---

## 1. Context — the consumer

qcalm projects a bank's balance sheet daily over a 3+ year horizon, per scenario. Its domain model splits responsibilities so that the library boundary and the domain-language boundary coincide:

- **`qcfinancial` owns the purely financial layer**: an *Operation* is the financial terms of a deal — one or more legs — and the contractual cashflows they generate. It knows nothing about clients, products, or accounting.
- **qcalm owns the business layer**: a *Contract* wraps an Operation with business identity (client, product, status); the *Bank* aggregates Contracts plus cash. qcalm references operations through opaque keys only.

qcalm's daily loop is event-driven: operations are **built once** (at snapshot load, or when a renewal/origination/prepayment creates one) and never regenerated. Each simulated day, the loop needs the **state of every leg of every live operation**. That state query is the hot path of the entire system — hence this requirement.

## 2. Requirement A — the `Operation` type

1. An `Operation` is a container of one or more `Leg` objects (the existing qcf `Leg`). Term deposits and typical loans have one leg; swaps have two; some loans need several to be modeled accurately.
2. Constructible from the outputs of the existing `LegFactory` builders (no new leg mechanics required for this).
3. Identified externally by an **opaque key** supplied by the caller (qcalm's `contract_id`); qcf attaches no meaning to it.
4. Legs within an operation are indexed (`leg_number`) and carry their direction (`rec_pay`), as today.
5. Immutable once built: restructurings are modeled by the caller as new Operations, never by mutating an existing one.

## 3. Requirement B — portfolio container

1. A container holding many Operations (target scale: 10⁵–10⁶), **built once and mutated incrementally**: the caller adds operations (renewals, new production) and removes them (maturities, prepayments) as the simulation advances. It is never rebuilt inside a projection.
2. The container is the receiver of all batch queries (Requirements C and D). Crossing the Python/C++ boundary must happen **once per query**, not once per operation.

## 4. Requirement C — batch state calculation (the hot path)

1. One call: *given the container and a date `t`, return the state of every leg of every operation.*
2. Per-leg state at `t`, minimally: accrued interest, outstanding notional, present value (of the remaining flows, given a discount curve passed to the query), next flow date, flows settling exactly at `t` (amount and type), and identification (`operation key`, `leg_number`).
2a. For present value, the query receives the discount curve(s) from the caller (`ZeroCouponCurve`), with a caller-supplied mapping from leg to curve (in F1, simply by currency). PV may be optional per query — the daily loop needs accrual and flows always, PV only on valuation dates.
3. Output must be **columnar** and consumable from Python without per-row object conversion — the caller feeds it directly into columnar aggregation (DuckDB/polars per qcalm's design doc §8.3).
4. The caller's dates are **monotonically increasing** within a projection (daily steps, forward only). The API may exploit this property.
5. **The calculation must be parallelizable across operations, and the implementation must parallelize it.** State per operation depends on nothing outside that operation. How to parallelize is out of scope here — resolve it with the qcf code in context.
6. Order-of-magnitude target: one projection = ~1,100 daily state queries over the full container; multiple scenario projections run concurrently on the same machine. The state query must be cheap enough that it does not dominate the projection loop.

## 5. Requirement D — batch flow window (second consumer, near-term)

1. Same container, one call: *given dates `t1 < t2`, return all contractual flows settling in `(t1, t2]` for every leg of every operation*, columnar as in C.
2. Motivation: the regulatory liquidity ratio (RCL) is computed inside the projection as a stressed 30-day window over projected flows, every day. Designing the container's query surface with this second consumer in mind avoids a second round of API design in a few months.

## 6. Constraints

1. **Purely financial**: no client, product, or accounting concepts enter qcf. Opaque keys only.
2. **Determinism**: identical inputs produce identical outputs; results must be reproducible against a pinned wheel version (qcalm records the wheel in every run's lineage).
3. **Rounding**: internal calculations in raw doubles; currency rounding (`QCCurrency.amount()`) is applied by the caller at the settlement edge only.
4. **Scope**: F1 consumes only single-leg operations built from `FixedRateLeg` (bullet and custom amortization). The design should not preclude any existing leg/cashflow type, but only fixed-rate needs to be exercised now.

## 7. Acceptance sketch

- Build a container with 10⁶ single-leg fixed-rate operations; query state daily over 3 years; verify per-leg accrual against `FixedRateCashflow.accrued_interest` on sampled operations.
- Incremental add/remove during the date walk (simulating renewals and maturities) with correct results after each mutation.
- Round-trip: an Operation built from serialized terms reproduces the same cashflow schedule (`show`/`record`) as one built directly.

---

## Appendix — what "columnar output" means

Instead of returning a list of state objects (one per leg), the batch query returns a set of **parallel arrays, one per field, all the same length** — element *i* of every array refers to the same leg.

For a container with N live legs, the result of `states_at(portfolio, t)` looks like:

```
op_key:              [1001, 1001, 1002, 1003, ...]   # int64,  length N
leg_number:          [   1,    2,    1,    1, ...]   # int32,  length N
accrued_interest:    [12.3, -8.1, 40.2,  0.9, ...]   # double, length N
outstanding_notional:[ ...                      ]    # double, length N
present_value:       [ ...                      ]    # double, length N (if requested)
next_flow_date:      [ ...                      ]    # int32/date, length N
flow_settling_today: [ 0.0,  0.0, 105.2, 0.0, ...]   # double, length N
```

The row `(1002, 1, 40.2, ..., 105.2)` — read across the arrays at the same index — is one leg's state, but no row object ever exists.

**Why it matters at the boundary.** If C++ returns N structs, the binding layer converts each into a Python object — N conversions, N heap allocations — and the consumer immediately re-columnarizes them anyway to aggregate. If C++ instead fills one contiguous buffer per field, Python receives ~10 array handles regardless of N (buffer protocol or Arrow): the data is not copied, Python gets typed views over memory C++ allocated. That is the "one boundary crossing per query" property of §4.3 — cost O(fields), not O(legs).

**Why it matters downstream.** "Sum accrued interest by currency over 10⁶ legs" becomes a vectorized reduction over one contiguous double array — cache-friendly, SIMD-able — rather than 10⁶ attribute lookups on objects.

**Left open deliberately**: the concrete carrier for the arrays (raw numpy-compatible buffers via the buffer protocol vs. Arrow arrays). Resolve in openspec with the qcf code in context; the requirement is only the parallel-array shape and the zero-per-row-conversion property.
