# portfolio-batch-queries Specification

## Purpose

The columnar batch query surface on `Portfolio`: `states_at(t, curves?)` for per-leg state at a date, and `flows_between(t1, t2)` for every flow settling in a window. Covers the output schema, row ordering, the one-boundary-crossing contract (numpy arrays, no per-row conversion), determinism regardless of thread count, and parallel execution semantics.
## Requirements
### Requirement: Columnar state query states_at
`Portfolio.states_at(t, curves=None)` SHALL return, in one call, the state of every leg of every operation as parallel numpy arrays (dict of column name → array, all of length N = total live legs). Columns SHALL include: `op_key` (int64), `leg_number` (int32), currency identification, `accrued_interest`, `outstanding_notional`, `interest_settling`, `amortization_settling`, `total_settling`, `next_flow_date` (int64 excel serial, 0 if none), and `present_value` (double, NaN where not computed). No per-row Python object SHALL be created; the boundary cost SHALL be O(columns), not O(legs).

#### Scenario: State of a fixed-rate leg mid-period
- **WHEN** `states_at(t)` runs with `t` strictly inside a coupon period of a fixed-rate leg
- **THEN** the leg's row reports `accrued_interest` equal to `FixedRateCashflow::accruedInterest(t)` of the period's cashflow, `outstanding_notional` equal to that cashflow's nominal, `next_flow_date` equal to the excel serial of the next settlement date after `t`, and zero in all settling columns

#### Scenario: Flows settling exactly at t
- **WHEN** `t` equals a settlement date of an amortizing fixed-rate cashflow
- **THEN** `total_settling` equals its raw unrounded `amount()`, `amortization_settling` equals its amortization, `interest_settling` equals the difference, and the settling cashflow is excluded from `present_value`

#### Scenario: Outside the leg's life
- **WHEN** `t` is before the leg's first start date or on/after its last end date
- **THEN** `accrued_interest` and `outstanding_notional` are 0.0

#### Scenario: Columnar boundary
- **WHEN** `states_at` returns for a portfolio of any size
- **THEN** the result is a dict of numpy arrays sharing index alignment (element i of every array describes the same leg)

### Requirement: Optional present value with caller-supplied curves
When `states_at` receives a mapping from currency ISO code to `ZeroCouponCurve`, it SHALL compute `present_value` per leg as the sum over cashflows with settlement date strictly after `t` of raw unrounded `amount() × df(settlementDate − t)`, using the curve matching the leg's currency. Legs whose currency has no supplied curve SHALL get NaN. When no mapping is supplied, PV computation SHALL be skipped entirely and the column filled with NaN. Internal calculations SHALL use raw doubles with no currency rounding.

#### Scenario: PV matches per-cashflow discounting
- **WHEN** `states_at(t, curves={"CLP": curve})` runs on a CLP fixed-rate leg
- **THEN** the leg's `present_value` equals the sum of each remaining cashflow's raw `amount()` discounted with `curve.getDiscountFactorAt(settlementDate − t)` within double-precision tolerance

#### Scenario: Missing curve yields NaN
- **WHEN** curves are supplied but the leg's currency is not in the mapping
- **THEN** that leg's `present_value` is NaN and every other column is still computed

### Requirement: Columnar flow window flows_between
`Portfolio.flows_between(t1, t2)` SHALL return, in one call, every contractual flow with settlement date in `(t1, t2]` across all legs, as parallel numpy arrays with columns: `op_key`, `leg_number`, `settlement_date` (int64 excel serial), `interest`, `amortization`, `total`, and currency identification. Rows SHALL be ordered by `(op_key, leg_number, settlement_date)`.

#### Scenario: Window boundaries
- **WHEN** a leg has flows settling at `t1`, at `t1+1d`, and at `t2`
- **THEN** the output excludes the flow at `t1` and includes the flows at `t1+1d` and `t2`

#### Scenario: Empty window
- **WHEN** no flow of any leg settles in `(t1, t2]`
- **THEN** the result is the same dict of columns with length-0 arrays

### Requirement: Parallel execution with deterministic results
Batch queries SHALL release the GIL and parallelize across operations using `std::thread` (no OpenMP, no new dependencies). Results SHALL be bitwise identical for identical inputs regardless of thread count, and identical across runs against the same wheel version.

#### Scenario: Thread count does not change output
- **WHEN** the same query runs on the same portfolio with parallelism forced to 1 thread and to N threads
- **THEN** every output array is bitwise identical

#### Scenario: Shared curves are not mutated concurrently
- **WHEN** a PV query runs in parallel with curves supplied
- **THEN** worker threads perform no calls on `ZeroCouponCurve`/`PresentValue` objects (discount factors are precomputed serially per query), so no data race exists on curve state

### Requirement: Batch scale and mutation correctness
Batch queries SHALL remain correct under the acceptance workload: a portfolio at the 10⁵–10⁶ operation scale queried daily over a 3-year horizon with interleaved add/remove between queries.

#### Scenario: Date walk with interleaved mutation
- **WHEN** a daily date walk queries `states_at` while operations are added (renewals) and removed (maturities) between days
- **THEN** each day's output reflects exactly the operations present at query time, with per-leg accrual matching `FixedRateCashflow::accruedInterest` on sampled operations

