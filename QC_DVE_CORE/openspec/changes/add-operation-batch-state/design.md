# Design: add-operation-batch-state

## Context

qcalm's daily projection loop needs per-leg state (accrual, outstanding notional, optional PV, next/settling flows) for 10⁵–10⁶ operations, once per simulated day, ~1,100 days per projection, columnar output, one Python/C++ boundary crossing per query. Requirements: `requirements/qcf_requirements_operation_state_v0.1_EN.md`.

Current state of the codebase relevant to this design:

- `Leg` is `std::vector<std::shared_ptr<Cashflow>>` with date-ordered cashflows; `FixedRateCashflow` already provides `accruedInterest(const QCDate&)`, `getNominal()`, `getAmortization()`, `settlementAmount()`, `getSettlementDate()`.
- The `Cashflow` base exposes only `amount()/ccy()/date()/startDate()/endDate()` — no uniform accrual/notional surface.
- **The entire curve query chain mutates shared state**: `ZeroCouponCurve::getDiscountFactorAt` writes `_dfDerivatives`/`_wfDerivatives` and calls `_intRate.setValue(...)`; `QCLinearInterpolator::interpolateAt` rewrites `_derivatives`. `PresentValue::pv` additionally writes into the cashflow (`savePresentValue`). None of it is thread-safe.
- No parallelism and no numpy bindings exist anywhere in the repo today.
- `QCDate::excelSerial()` provides a stable integer date encoding.

Constraints from the caller: no OpenMP (explicit), no new third-party dependencies, determinism against a pinned wheel, F1 exercises only single-leg fixed-rate operations, dates within a projection are monotonically increasing.

## Goals / Non-Goals

**Goals:**

- `Operation` and `Portfolio` types with incremental add/remove by opaque `int64` key.
- `states_at(t, curves?)` and `flows_between(t1, t2)` returning parallel numpy arrays, one GIL-released boundary crossing per query.
- Parallel across operations with plain `std::thread`; identical results regardless of thread count.
- Correct fixed-rate state (bullet + custom amortization) verified against `FixedRateCashflow::accruedInterest`.
- Design does not preclude other cashflow types (uniform virtual surface with safe defaults).

**Non-Goals:**

- OpenMP, Arrow, or any new dependency.
- DV01/derivatives in batch queries (that stays with `PresentValue`).
- Forward-rate fixing for floating legs (F1 is fixed-rate; `ForwardRates` untouched).
- Serialization of Operations (round-trip in the acceptance sketch is built-from-terms, which `LegFactory` already covers).
- Any client/product/accounting concept.

## Decisions

### D1 — Uniform state surface: virtuals on `Cashflow` with inert defaults

Add to `Cashflow`:

```cpp
virtual double accruedInterest(const QCDate& d) { return 0.0; }   // matches FixedRateCashflow's existing signature
virtual double getNominal() const { return 0.0; }
virtual QCDate getSettlementDate() const { return date(); }       // only if signatures require harmonizing
```

`FixedRateCashflow` already defines matching methods, so it overrides automatically (add `override` keywords). Other types keep the inert default until someone needs them — the batch engine then works for any leg type whose cashflows override, without downcasts.

*Alternative rejected:* `dynamic_cast<FixedRateCashflow*>` fast path in the engine — smaller today, but bakes F1 scope into the hot loop and precludes other leg types (violates requirement §6.4).

*Alternative rejected:* pure virtuals — would force implementing (or stubbing) them on every one of ~12 cashflow types in this change.

### D2 — Thread-safe PV via per-query discount-factor table (curve classes untouched)

Since every curve method mutates, do not call curves from worker threads at all. Per query, per curve, **serially** precompute `df[day]` for `day = 0 .. maxHorizonDays` (offset from `t`, using the existing mutating `getDiscountFactorAt`), into a `std::vector<double>`. Worker threads only read the table: `pv = Σ settlementAmount × df[settlementDate − t]`.

Size: a 30-year horizon is ~11,000 doubles per curve — microseconds to build, ~88 KB. This is strictly cheaper and safer than adding a parallel const path through curve + interpolator + `QCInterestRate` (three classes to rework) or deep-cloning curves per thread (no clone support exists).

Curve mapping: the query takes a Python dict `{currency_iso_code: ZeroCouponCurve}`; each leg resolves its curve by `cashflow->ccy()->getIsoCode()`. PV columns are filled with NaN when no curve is supplied for a leg's currency (and the whole PV computation is skipped when the dict is absent — the daily loop needs accrual/flows only).

### D3 — Data model: `Operation` + `Portfolio`

```cpp
class Operation {           // immutable after construction
    long long key;                       // opaque, caller-supplied
    std::vector<Leg> legs;               // leg_number = index + 1
    std::vector<RecPay> recPay;          // metadata per leg; amounts keep their signs as built by LegFactory
};

class Portfolio {
    std::map<long long, Operation> ops;  // ordered → deterministic row order, O(log n) add/remove
};
```

- `std::map` keyed by operation key gives deterministic iteration (rows always sorted by `(op_key, leg_number)`) with no extra sort step. At 10⁶ ops the per-query O(N) walk to build a flat snapshot is milliseconds and is needed under any container choice.
- Duplicate key on add and missing key on remove throw `std::invalid_argument` (surfaces as Python `ValueError`).
- Immutability is API-level: no setters on `Operation`; restructurings are remove + add by the caller.

### D4 — Per-leg state semantics (the conventions the spec pins down)

For a leg at date `t`, with cashflows ordered by period:

- **Current period**: the cashflow with `startDate ≤ t < endDate`, found by `std::upper_bound` on end dates (~6 comparisons for a 40-period leg). No monotone cursor — requirement §4.4 *allows* exploiting monotone dates but binary search is already negligible; revisit only if profiling says otherwise.
- **Accrued interest**: `currentCashflow->accruedInterest(t)`; `0.0` if `t` is before the first `startDate` or on/after the last `endDate`.
- **Outstanding notional**: `currentCashflow->getNominal()`; `0.0` outside the leg's life (same boundary convention as accrual).
- **Next flow date**: smallest `settlementDate > t`, as `excelSerial()` int64; `0` if none remain.
- **Flow settling at `t`**: sum over cashflows with `settlementDate == t` of raw `amount()` — NOT `settlementAmount()`, which applies currency rounding and would violate requirement §6.3 (raw doubles; the caller rounds at the settlement edge). Split: `interest_settling = Σ accruedInterest(endDate)` (bitwise-identical to the cashflow's internal `_interest`) and `amortization_settling = total − interest_settling`. This derivation needs no `getAmortization`/`doesAmortize` virtuals on the base. Zero when nothing settles.
- **Present value**: `Σ_{settlementDate > t} amount() × df[settlementDate − t]` (raw, unrounded). Flows settling exactly at `t` are excluded from PV (they are reported in the settling columns — the caller books them as cash).
- All values are raw doubles; no currency rounding anywhere in the engine (requirement §6.3 — rounding is the caller's job at the settlement edge).

### D5 — Columnar output: preallocated `py::array_t`, threads write disjoint ranges

`states_at` knows N (total live legs) up front: allocate every column as `py::array_t` once, release the GIL (`py::gil_scoped_release`), chunk the flat leg snapshot across `min(std::thread::hardware_concurrency(), nChunks)` `std::thread`s, each thread writing its own index range of the raw buffers. No locks, no false-sharing concern at these sizes, bitwise-deterministic because each output element depends only on its own leg.

Columns for `states_at`: `op_key:int64, leg_number:int32, currency:fixed-width str or int index, accrued_interest, outstanding_notional, interest_settling, amortization_settling, total_settling, next_flow_date:int64, present_value (NaN-filled when not requested)`. Returned as a Python dict of arrays — feeds `polars.DataFrame(dict)` / DuckDB directly, zero copies.

`flows_between(t1, t2)` has variable rows per leg, so it runs two passes: (1) parallel count of flows per leg in `(t1, t2]` → prefix-sum to per-leg offsets, (2) allocate exact-size arrays, parallel fill of disjoint ranges. Columns: `op_key, leg_number, settlement_date:int64, interest, amortization, total, currency`. Same mechanism, same determinism argument.

*Alternative rejected:* Arrow output — new dependency for no F1 gain; numpy buffer protocol already satisfies the "O(fields) boundary" requirement, and polars/DuckDB ingest numpy directly.

*Alternative rejected:* per-thread local buffers + concatenate for `flows_between` — nondeterministic ordering unless re-sorted; count-then-fill keeps rows in portfolio order for free.

### D6 — Parallelism: chunked `std::thread`, no pool, no OpenMP

A query spawns ≤ hardware_concurrency threads, joins them, done. Thread creation cost (~µs each) is noise against a 10⁶-leg scan. No thread pool until profiling demands one; no OpenMP ever (explicit constraint — also avoids the libomp wheel-distribution problem on macOS/Linux).

Multiple concurrent scenario projections (requirement §4.6) are separate Python processes with separate Portfolios — no cross-process concern in qcf.

### D7 — File layout and bindings

- `include/portfolio/Operation.h`, `include/portfolio/Portfolio.h`, `source/portfolio/Portfolio.cpp` (engine + queries; Operation is header-only if trivial).
- Register `.cpp` in `source/CMakeLists.txt`; helpers `registerOperation` / `registerPortfolio` in `QcfinancialPybind11Helpers.h`; called from `source/qcf_binder.cpp`. `#include <pybind11/numpy.h>` — header-only, already in the submodule.
- Python surface (F1):

```python
op = qcf.Operation(key=1001, legs=[leg], rec_pay=[qcf.RecPay.RECEIVE])
port = qcf.Portfolio()
port.add(op); port.remove(1001)
state = port.states_at(t, curves={"CLP": curve})          # dict[str, np.ndarray]
flows = port.flows_between(t1, t2)                        # dict[str, np.ndarray]
```

## Risks / Trade-offs

- [Virtual-call overhead in the hot loop: ~2 virtual calls per leg per day] → Negligible at 10⁹ leg-states/projection (~seconds parallelized); measured before optimizing further. The df table already removes the expensive part.
- [Inert defaults on `Cashflow` silently return 0.0 for leg types that never overrode them] → F1 builds Portfolios only from fixed-rate legs; the spec records the convention, and extending a new type is a deliberate override, not a cast site. Accepted over pure virtuals (touching 12 types now).
- [`std::map` iteration is pointer-chasing at 10⁶ entries] → The flat snapshot is rebuilt per query in O(N) milliseconds; if profiling flags it, a cached snapshot invalidated on add/remove is a drop-in change behind the same API.
- [df table indexes by integer day offset — settlement dates beyond the table are out of range] → Table sized to the portfolio's max settlement date per query, computed during the count pass; assert-guarded.
- [Currency column as strings costs conversions] → Emit a small `int32` currency index plus a per-query `list[str]` legend (O(#currencies)); columnar consumers join it trivially.
- [`accruedInterest` on `FixedRateCashflow` is non-const] → Base virtual mirrors the existing non-const signature; worker threads each own disjoint operations, so mutation inside a cashflow (none expected for fixed-rate accrual) cannot race across threads.

## Open Questions

- None blocking. The `rec_pay` metadata is stored and echoed but signs remain baked into amounts by `LegFactory` (as today); if qcalm later wants unsigned amounts + direction column, that is an additive column change.
