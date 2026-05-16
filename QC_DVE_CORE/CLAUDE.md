# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

QC_DVE_CORE is a C++17 library for valuation of linear interest rate and FX derivatives, exposed to Python via pybind11 as the `qcfinancial` package. It includes Chilean market-specific instruments (ICP-CLP, ICP-CLF/UF).

Current version: **1.10.1** (set in `setup.py`).

## Branch Strategy

- **`master`** — stable releases only; merges come from `develop` via PR.
- **`develop`** — active development branch; new features and fixes land here first.

Always work on `develop` (or a feature branch off `develop`) and merge to `master` when ready to release.

## Build Commands

### Build Python wheel (primary workflow)

```bash
# Build for current pyenv Python version
python setup.py bdist_wheel

# Build for a specific pyenv version
./compile.sh 3.12.1

# Build for all supported versions (3.10.13, 3.11.5, 3.12.1, 3.13.1)
./compile.sh
```

Wheels land in `dist/`. The build uses CMake + Ninja under the hood via `setup.py`'s `CMakeBuild` extension.

### CMake directly (for IDE / debug builds)

CLion-style build directories exist: `cmake-build-debug/` and `cmake-build-release/`.

```bash
# Configure
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release -GNinja

# Build static library + Python extension
cmake --build cmake-build-release
```

The CMake build produces:
- `QC_DVE_CORE.a` — static library
- `qcfinancial.cpython-<ver>-darwin.so` — Python extension module

### macOS SDK note

On macOS, `CMakeLists.txt` automatically detects the SDK path via `xcrun --show-sdk-path` to handle Xcode CLT changes that moved C++ stdlib headers.

## C++ Tests

Tests use the **Catch2** framework (v2, header-only at `Tests/include/catch/catch-2.hpp`). The test executable target is currently commented out in `Tests/CMakeLists.txt`. To re-enable, uncomment the `target_sources` block in that file and add a corresponding `add_executable` + `target_link_libraries` in the root `CMakeLists.txt`.

Test files are in `Tests/` (e.g., `QCDateTests.cpp`, `LegFactoryTests.cpp`). Each file uses `#include "catch/catch-2.hpp"` and `TEST_CASE` / `SECTION` / `REQUIRE` macros.

## Architecture

The library is organized in layers; each layer depends only on layers below it.

### Layer 1 — Time (`include/time/`, `source/time/`)

- `QCDate` — date representation and arithmetic (day count, business day adjustment)
- `QCBusinessCalendar` — holiday calendars and business day conventions

### Layer 2 — Asset Classes (`include/asset_classes/`, `source/asset_classes/`)

Foundational financial primitives:

- **Currencies**: `QCCurrency` and subclasses (CLP, USD, EUR, CLF/UF, etc.)
- **Year fractions**: `QCYearFraction` base + `QCAct360`, `QCAct365`, `QCActAct`, `QC30360`, `QC3030`, `QCAct30`
- **Wealth factors / compounding**: `QCWealthFactor` base + `QCLinearWf` (simple), `QCCompoundWf` (compound), `QCContinousWf` (continuous)
- **Interest rate**: `QCInterestRate` — bundles a value, a year fraction, and a wealth factor
- **Tenor**: `Tenor` — period representation (e.g. 6M, 1Y)
- **Indices**: `InterestRateIndex`, `FXRate`, `FXRateIndex`, `IndexFactory`
- **Curve interface**: `InterestRateCurve` (abstract), `ZeroCouponCurve` (concrete with derivatives for sensitivities)

### Layer 3 — Curves (`include/curves/`)

Curve implementations with interpolation:

- `QCCurve<T>` — generic curve template over a sorted knot vector
- Interpolators: `QCLinearInterpolator`, `QCLogLinearInterpolator`, `QCClampedSpline`
- `QCZeroCouponDiscountFactorCurve`, `QCZeroCouponInterestRateCurve`, `QCProjectingInterestRateCurve`

### Layer 4 — Cashflows (`include/cashflows/`, `source/cashflows/`)

All cashflow types share the abstract `Cashflow` interface (`amount()`, `ccy()`, `date()`).

| Class | Description |
|---|---|
| `SimpleCashflow` | Single deterministic payment |
| `SimpleMultiCurrencyCashflow` | Simple payment with FX conversion |
| `FixedRateCashflow` | Fixed rate interest |
| `FixedRateMultiCurrencyCashflow` | Fixed rate with FX conversion |
| `IborCashflow` | Floating IBOR-style |
| `IborMultiCurrencyCashflow` | IBOR with FX conversion |
| `IcpClpCashflow` | Chilean overnight index (ICP) in CLP |
| `IcpClfCashflow` | Chilean overnight index in CLF (UF) |
| `CompoundedOvernightRateCashflow2` | Generic compounded overnight rate with curve sensitivity support |
| `CompoundedOvernightRateMultiCurrencyCashflow2` | Compounded overnight with FX conversion |
| `OvernightIndexCashflow` | Overnight index (SOFR, SONIA, etc.) with settlement flexibility |
| `OvernightIndexMultiCurrencyCashflow` | Overnight index with FX conversion |

All active cashflow types support a `record()` method returning a `std::tuple` of all cashflow fields (used by the Python bindings for DataFrame construction). The `record()` tuple always includes `present_value` and `discount_factor` as its final two fields.

> **Deprecated (moved to `deprecated/`):** `LinearInterestRateCashflow`, `FixedRateCashflow2`, `IborCashflow2`, `IcpClpCashflow2`, `CompoundedOvernightRateCashflow` (without `2`), `QuantoLinearInterestRateCashflow`. Do not use these types in new code.

### Layer 5 — Legs and Factory (`include/Leg.h`, `include/LegFactory.h`)

- `Leg` — ordered container of `shared_ptr<Cashflow>`, accessible by index
- `LegFactory` — static factory methods (`buildFixedRateLeg`, `buildIborLeg`, `buildIcpClpLeg`, etc.) that construct full legs from schedule parameters

### Layer 6 — Present Value / Pricing (`include/present_value/`)

- `PresentValue` — discounts a `Leg` against a `ZeroCouponCurve`; computes first-order derivatives (DV01) w.r.t. curve nodes
- `ForwardRates` — sets forward rates on `IborCashflow`, `IcpClfCashflow`, `CompoundedOvernightRateCashflow2`, `OvernightIndexCashflow`, and their multi-currency variants using projection curves
- `ForwardFXRates` — FX forward estimation from two discount curves (`ForwardFXRates.cpp`)
- `FXRateEstimator` — spot FX + basis point adjustments (`FXRateEstimator.cpp`)

### Python Bindings

- `source/qcf_binder.cpp` — the sole active pybind11 module (`PYBIND11_MODULE(qcfinancial, m)`)
- `qcfinancial/qcfinancial_core.cpp` — legacy binder compiled as a separate module by `setup.py`; do not add new bindings here

The `QcfinancialPybind11Helpers.h` header contains helper registration functions called from the binder (one function per class). When adding a new C++ class, add its `.cpp` to `source/CMakeLists.txt` under `target_sources(QC_DVE_CORE ...)`, expose it in `source/qcf_binder.cpp`, and register it via a helper in `QcfinancialPybind11Helpers.h`.

Opaque STL bindings (`PYBIND11_MAKE_OPAQUE`) are declared at the top of `qcf_binder.cpp` before any includes that use those types. Common shared type aliases live in `include/TypeAliases.h`; opaque type declarations in `include/PybindOpaqueTypes.h`.

### Submodule Dependencies

- `pybind11/` — Python/C++ binding library
- `eigen/` — linear algebra (used in curve bootstrapping)
- `autodiff/` — automatic differentiation (used for sensitivity calculations)

## Adding a New Feature (Checklist)

Use this checklist when adding a new cashflow type or other significant feature:

1. **Header** — add `include/cashflows/MyNewCashflow.h` (or the appropriate layer).
2. **Implementation** — add `source/cashflows/MyNewCashflow.cpp`.
3. **Register in build** — add the `.cpp` path to `source/CMakeLists.txt` under `target_sources(QC_DVE_CORE ...)`.
4. **Python binding helper** — add a `registerMyNewCashflow(py::module& m)` function in `QcfinancialPybind11Helpers.h`.
5. **Expose in binder** — call the helper from `source/qcf_binder.cpp`.
6. **`record()` tuple** — if the type supports `record()`, ensure the final two fields are `present_value` and `discount_factor` (convention established in v1.10.1).
7. **Test** — add or update a test file in `Tests/`.
8. **Version bump** — update `version=` in `setup.py`.
9. **Commit message** — follow the pattern `# Update to Version X.Y.Z: <description>`.

## Versioning

Version lives in `setup.py` (`version="1.10.2"`). Bump it there when releasing. Commit messages follow the pattern `# Update to Version X.Y.Z: <description>`.

**Never** add a `Co-Authored-By: Claude ...` trailer to commit messages.
