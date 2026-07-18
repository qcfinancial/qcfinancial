# Tasks: add-operation-batch-state

## 1. Uniform state surface on Cashflow

- [x] 1.1 Add inert virtuals to `include/cashflows/Cashflow.h`: `accruedInterest(const QCDate&)` → 0.0, `getNominal() const` → 0.0, plus a settlement-date accessor if signature harmonizing is needed (design D1)
- [x] 1.2 Mark the matching methods in `FixedRateCashflow` as `override`; verify signatures line up exactly (build catches mismatches)
- [x] 1.3 Full rebuild of `QC_DVE_CORE` to confirm no other cashflow type accidentally collides with the new virtuals

## 2. Operation and Portfolio types

- [x] 2.1 Create `include/portfolio/Operation.h`: key, `vector<Leg>`, `vector<RecPay>`, 1-based `leg_number`; construction validation (non-empty legs, rec_pay length match → `std::invalid_argument`); accessors only, no mutators
- [x] 2.2 Create `include/portfolio/Portfolio.h` + `source/portfolio/Portfolio.cpp`: `std::map<long long, Operation>`, `add` (duplicate key throws), `remove` (missing key throws), `size`
- [x] 2.3 Register `source/portfolio/Portfolio.cpp` in `source/CMakeLists.txt` under `target_sources(QC_DVE_CORE ...)`

## 3. Batch state engine (C++)

- [x] 3.1 Implement per-leg state at `t` (design D4): binary search for current period; accrued interest, outstanding notional, next flow date (excel serial), settling-flow split (total / amortization / interest); 0.0 conventions outside leg life
- [x] 3.2 Implement per-query discount-factor table (design D2): serial precompute `df[0..maxHorizonDays]` per currency from the supplied `ZeroCouponCurve`s; PV per leg as Σ settlementAmount × df[offset], flows at `t` excluded; NaN for missing curve or absent mapping
- [x] 3.3 Implement `states_at` driver (design D5/D6): flat `(key, leg)` snapshot in map order, preallocated output buffers, chunked `std::thread` workers writing disjoint ranges
- [x] 3.4 Implement `flows_between(t1, t2]` (design D5): parallel count pass → prefix-sum offsets → parallel fill pass; rows ordered `(op_key, leg_number, settlement_date)`

## 4. Python bindings

- [x] 4.1 Add `registerOperation` and `registerPortfolio` in `QcfinancialPybind11Helpers.h`; call from `source/qcf_binder.cpp`; include `pybind11/numpy.h`
- [x] 4.2 Bind `states_at(t, curves: dict[str, ZeroCouponCurve] | None)` and `flows_between(t1, t2)` returning dict of `py::array_t` columns; wrap the C++ compute in `py::gil_scoped_release`; map `std::invalid_argument` → `ValueError`
- [x] 4.3 Emit currency as int32 index column + per-query legend list (design D5 risk note)

## 5. Tests

- [x] 5.1 C++ Catch2 test `Tests/PortfolioTests.cpp`: operation construction/validation, portfolio add/remove/duplicate/missing, per-leg state vs `FixedRateCashflow::accruedInterest` for bullet and custom-amortization legs, settling-flow split, window boundary `(t1, t2]`, PV vs per-cashflow discounting
- [x] 5.2 Determinism check: same query with 1 thread vs N threads, bitwise-identical outputs (expose or inject thread count for testability)
- [x] 5.3 Python-level check (script or test): build wheel, portfolio with mixed insertion order → row order `(op_key, leg_number)`; numpy dict feeds `polars.DataFrame` without conversion; NaN conventions for PV
- [x] 5.4 Scale smoke: ~10⁵–10⁶ single-leg fixed-rate ops, daily `states_at` walk over a sampled multi-year range with interleaved add/remove; record rough timing to confirm the query does not dominate the loop

## 6. Release

- [x] 6.1 Bump `version=` in `setup.py` (1.12.0 — new API surface) and update CLAUDE.md current-version note
- [x] 6.2 Build wheel for the default pyenv Python and run the Python-level checks against it
- [x] 6.3 Commit following `# Update to Version 1.12.0: <description>` (no Co-Authored-By)
