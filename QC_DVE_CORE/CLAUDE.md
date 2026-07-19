# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

QC_DVE_CORE is a C++17 library for valuation of linear interest rate and FX derivatives, exposed to Python via pybind11 as the `qcfinancial` package. It includes Chilean market-specific instruments (ICP-CLP, ICP-CLF/UF).

Current version: **1.12.0a3** (set in `setup.py`).

## Branch Strategy

- **`master`** — stable releases only; merges come from `develop` via PR.
- **`develop`** — active development branch; new features and fixes land here first.

Always work on `develop` (or a feature branch off `develop`) and merge to `master` when ready to release.

The `1.12.0` series was developed on an `alm` branch through 2026-07-19; it was merged back into `develop` and retired. No branch other than the two above is active.

## Development Machines

This project is developed on two machines with different architectures:

| Machine | CPU | Architecture |
|---|---|---|
| Mac (primary) | Apple M2 | `arm64` |
| Mac (secondary) | Intel | `x86_64` |

Wheels built on each machine are architecture-specific (e.g. `macosx_26_0_arm64` vs `macosx_XX_0_x86_64`). The pyenv setup below refers to the **Apple Silicon (M2) machine**. The Intel machine may have a different set of installed versions.

### pyenv setup (Apple Silicon / M2)

Active pyenv versions as of June 2026:

| Version | Role |
|---|---|
| `3.11.15` | Supported build target |
| `3.12.13` | Supported build target; **global default** |
| `3.13.13` | Supported build target |
| `3.14.5` | Supported build target |

All four versions have `setuptools`, `wheel`, and `ninja` installed. To set up a freshly installed version:

```bash
PYENV_VERSION=3.x.y pyenv exec python -m pip install setuptools wheel ninja
```

## Build Commands

### Build Python wheel (primary workflow)

```bash
# Build for current pyenv Python version
python setup.py bdist_wheel

# Build for a specific pyenv version
./compile.sh 3.12.13

# Build for all supported versions (3.11.15, 3.12.13, 3.13.13, 3.14.5)
./compile.sh
```

Wheels land in `dist/`. The build uses CMake + Ninja under the hood via `setup.py`'s `CMakeBuild` extension.

**Build prerequisites per pyenv version:** each target interpreter needs `setuptools`, `wheel`, and `ninja` installed (`<python> -m pip install setuptools wheel ninja`). Python **3.12+** no longer bundles `setuptools`, so a freshly installed pyenv 3.12/3.13/3.14 will fail with `ModuleNotFoundError: No module named 'setuptools'` until these are installed. (Python 3.11 still bundles setuptools.)

> **Note:** the `qcfinancial_py312` CMake target is a dev/CLion-only convenience (marked `EXCLUDE_FROM_ALL`, so it is *not* part of the wheel/`cmake --build .` flow) and hard-codes a specific pyenv path (currently `3.12.13`). Update that path in `CMakeLists.txt` when the installed 3.12 patch changes.

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

- `QCDate` — date representation and arithmetic (day counts, month/day offsets, `shift`, `monthDiffDayRemainder`)
- `QCBusinessCalendar` — holiday calendars, business-day conventions, **and business-day adjustment**: `businessDay(date, rule)`, `nextBusinessDay`, `previousBusinessDay`, `modNextBusinessDay`, `shift`

Adjustment belongs on the calendar because it needs the holiday set. Until `1.12.0a3` it also existed as `QCDate::businessDay(vector<QCDate>&, rule)`, which rebuilt an entire `QCBusinessCalendar` on every call to answer one lookup — that cost 43 ms per leg on a realistic 420-holiday calendar versus 0.24 ms now (180x). **Do not add calendar-taking overloads to `QCDate`, and do not flatten a calendar to `vector<QCDate>` to pass it down.** `QCDate` methods that need a calendar take `const QCBusinessCalendar&`.

`QCDate.business_day(holiday_list, rule)` survives in Python as a binder shim for compatibility and still rebuilds; prefer `BusinessCalendar.business_day(date, rule)`.

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

### Layer 7 — Portfolio / Batch State (`include/portfolio/`)

Added in 1.12.0 for columnar batch state queries over large portfolios.

- `Operation` — immutable container of one or more `Leg`s under a caller-supplied opaque key. qcfinancial attaches no meaning to the key. Legs are numbered from 1 in construction order.
- `Portfolio` — container of `Operation`s keyed by that key, mutated incrementally via `add`/`remove`. Exposes two batch queries returning parallel numpy arrays (zero-copy via `vectorToNumpy`):
  - `statesAt(t, curves, numThreads)` — per-leg accrual, outstanding notional, settling amounts, next flow date, optional present value
  - `flowsBetween(t1, t2, numThreads)` — every contractual flow settling in `(t1, t2]`

Both parallelize across operations with `std::thread` and are bitwise deterministic regardless of thread count. Curve objects are never touched from worker threads — the query chain (curve → interpolator → `QCInterestRate`) mutates internal state on every call, so discount factor tables are precomputed serially by day offset and the workers only index into them.

#### Current scope: fixed-rate legs only

The batch queries were built for phase F1, which covers **fixed-rate legs of simple commercial products only**. Two known limitations, both deliberate and both to be revisited:

1. **No forward-rate projection.** `statesAt` never calls `ForwardRates`; it discounts `cf->amount()` as-is. A floating leg therefore yields a `present_value` reflecting whatever rate is currently stored in the cashflow — correct only if the caller ran `ForwardRates` beforehand, and **silently wrong otherwise**. Unlike a missing discount curve, which correctly yields `NaN`, nothing in the output flags this.
2. **Discount curves are selected by currency ISO code.** Not a robust criterion: discounting follows the CSA, not the currency, so same-currency trades under different collateral agreements need different curves, and a cross-currency swap may discount each leg off a different curve.

The likely direction (discussed, not designed): attach projection and discount curve **names** to each leg, resolved against a name → curve map at query time, so curve objects stay out of the leg and the whole mapping can be swapped per scenario. Per-leg rather than per-operation, since a cross-currency swap's legs discount differently. The projection name may be redundant — cashflows already carry their `InterestRateIndex`, so projection curves could be keyed by index code with no new field. If projection is added, the serial-precompute constraint above applies: derive forwards in the workers from a precomputed discount factor table via `(df[start]/df[end] - 1) / yf` rather than calling curve objects from threads.

**Do not use `Portfolio` for floating-rate legs until limitation 1 is addressed.**

### Python Bindings

- `source/qcf_binder.cpp` — the sole active pybind11 module (`PYBIND11_MODULE(qcfinancial, m)`)
- `qcfinancial/qcfinancial_core.cpp` — legacy binder compiled as a separate module by `setup.py`; do not add new bindings here

The `QcfinancialPybind11Helpers.h` header contains helper registration functions called from the binder (one function per class). When adding a new C++ class, add its `.cpp` to `source/CMakeLists.txt` under `target_sources(QC_DVE_CORE ...)`, expose it in `source/qcf_binder.cpp`, and register it via a helper in `QcfinancialPybind11Helpers.h`.

Opaque STL bindings (`PYBIND11_MAKE_OPAQUE`) are declared at the top of `qcf_binder.cpp` before any includes that use those types. Common shared type aliases live in `include/TypeAliases.h`; opaque type declarations in `include/PybindOpaqueTypes.h`.

### Compiled-but-unreachable code

Several files a `grep` will hit are not in any build target, or are built but reachable from nothing. Check before investigating or refactoring them:

| File | Status |
|---|---|
| `source/QC_DVE_PYBIND.cpp` | in no target's source list |
| `include/QCDvePyBindHelperFunctions.h` | included only by `QC_DVE_PYBIND.cpp` |
| `source/QC_Financial.cpp` | listed in `QC_FINANCIAL_SOURCES` (`source/CMakeLists.txt:80`) — a variable **no target consumes** |

`QCFactoryFunctions.cpp` and `QCDiscountBondPayoff.cpp` *are* built and are unreferenced by `qcf_binder.cpp`, but are **not** safe to delete: `QCFXForward.cpp` and `QCTimeDepositPayoff.cpp` (both live) include `QCDiscountBondPayoff.h` and hold `shared_ptr<QCDiscountBondPayoff>` members, and `QCDiscountBondPayoff.cpp` includes `QCFactoryFunctions.h`. Removing them means removing that whole subtree, curve bootstrapping included.

The live source list is the `target_sources(QC_DVE_CORE ...)` block starting at `source/CMakeLists.txt:10`. "Not referenced by the binder" is not the same as "dead" — check every including file.

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
8. **Version bump** — update `version=` in `setup.py`, the version line in this file, **and the `id()` string in `source/qcf_binder.cpp`** (all three; the binder id is easy to miss).
9. **Commit message** — follow the pattern `# Update to Version X.Y.Z: <description>`.

## Versioning

Version lives in three places that must agree: `version=` in `setup.py`, the "Current version" line in this file, and the `id()` string in `source/qcf_binder.cpp`. Commit messages follow the pattern `# Update to Version X.Y.Z: <description>`.

**Never** add a `Co-Authored-By: Claude ...` trailer to commit messages.
