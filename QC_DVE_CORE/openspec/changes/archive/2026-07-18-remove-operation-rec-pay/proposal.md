## Why

`Operation` requires one `RecPay` per leg, but nothing in the library ever reads it: `Portfolio::statesAt` and `Portfolio::flowsBetween` compute exclusively from `cf->amount()`, whose signs `LegFactory` already baked in at leg construction. The parameter is stored, length-validated, exposed through `get_rec_pay`, asserted once in a test — and consumed by no calculation.

Worse, it is never validated against the legs it describes. A caller can pass `RECEIVE` for a leg whose amounts are all negative and the library will hand that contradiction back through `get_rec_pay`. The API is at `1.12.0a1`, so this is the cheapest moment it will ever be to remove.

## What Changes

- **BREAKING**: `Operation`'s constructor drops its third parameter. `Operation(key, legs, rec_pay)` becomes `Operation(key, legs)`, in both C++ and Python.
- **BREAKING**: `Operation::getRecPay` and the Python `get_rec_pay` binding are removed.
- The length-agreement validation between `legs` and `rec_pay` disappears with the parameter. The empty-legs and empty-leg validations stay.
- No back-compat overload or deprecation shim is provided — the API is alpha.
- Direction remains available to callers exactly where it already is: in the signs of the cashflow amounts, and in whatever the caller records alongside their opaque key.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `operation`: the construction requirement drops `rec_pay` from the constructor signature and from the accessor surface; the multi-leg and validation scenarios are restated without direction.

## Impact

- `include/portfolio/Operation.h` — constructor signature, `_recPay` member, `getRecPay` accessor, length validation.
- `source/qcf_binder.cpp` — `py::init` signature and the `get_rec_pay` binding (around line 1158).
- `Tests/PortfolioTests.cpp` — construction call sites and the `getRecPay` assertion (line 91).
- `qcfinancial-docs` repo, `8_Operaciones_Portafolio.ipynb` — `Operation` construction cells and the prose describing direction as metadata. This is a separate repository; the notebook must be re-executed after the new wheel is built.
- No impact on `Portfolio`, which never referenced the field.
- Callers already using the three-argument constructor must drop the third argument. Given the alpha status, the expected population is this repo and the docs notebook.
