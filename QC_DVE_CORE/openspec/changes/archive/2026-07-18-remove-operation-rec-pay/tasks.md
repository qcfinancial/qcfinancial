## 1. Core removal

- [x] 1.1 In `include/portfolio/Operation.h`, drop the `std::vector<RecPay> recPay` constructor parameter, the `_recPay` member, and its initializer
- [x] 1.2 In `include/portfolio/Operation.h`, delete the `_legs.size() != _recPay.size()` validation branch, leaving the empty-legs and empty-leg checks and their error text unchanged
- [x] 1.3 In `include/portfolio/Operation.h`, delete the `getRecPay` accessor and update the class doc comment, which currently states that legs "carry a RecPay direction as metadata"
- [x] 1.4 Confirm `Portfolio` needs no edit: `grep -rn "getRecPay\|_recPay" include source` returns nothing outside the removed lines

## 2. Python bindings

- [x] 2.1 In `source/qcf_binder.cpp`, change the `py::init` to `<long long, std::vector<qf::Leg>>` with `py::arg("key"), py::arg("legs")`
- [x] 2.2 In `source/qcf_binder.cpp`, remove the `.def("get_rec_pay", ...)` binding
- [x] 2.3 Update the `Operation` pybind docstring if it mentions direction

## 3. Tests

- [x] 3.1 In `Tests/PortfolioTests.cpp`, update every `Operation` construction to the two-argument form
- [x] 3.2 In `Tests/PortfolioTests.cpp`, delete the `REQUIRE(op.getRecPay(0) == RecPay::Receive)` assertion at line 91
- [x] 3.3 Add or adjust a case asserting that a two-leg operation built from a `RECEIVE` leg and a `PAY` leg keeps opposite amount signs, covering the restated multi-leg scenario
- [x] 3.4 Verify the zero-leg construction case still raises, covering the restated validation scenario
- [x] 3.5 (added during apply) Update `Tests/python/test_portfolio_batch.py`: three `Operation` construction sites and the `get_rec_pay` assertion, which the original task list missed

## 4. Build and verify

- [x] 4.1 Build the wheel for the current pyenv version (`python setup.py bdist_wheel`)
- [x] 4.2 Smoke-test in Python: `qcf.Operation(1, [leg])` constructs, `get_rec_pay` no longer exists, and `Portfolio.states_at` returns the same numbers as before the change for a fixed input
- [x] 4.3 Bump `version=` in `setup.py` and the `id()` string in `source/qcf_binder.cpp`, and update the version line in `CLAUDE.md`

## 5. Documentation

- [x] 5.1 In the `qcfinancial-docs` repo, update `8_Operaciones_Portafolio.ipynb`: the `Operation` construction cells, the `build_operacion` helper, the `get_rec_pay` print cell, and the prose describing direction as metadata
- [x] 5.2 Re-execute the notebook against the new wheel so its saved outputs match the shipped API
