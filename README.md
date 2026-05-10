<p align="center">
  <img src="https://img.shields.io/pypi/v/qcfinancial?color=blue&label=PyPI&logo=pypi&logoColor=white" alt="PyPI version"/>
  <img src="https://img.shields.io/pypi/pyversions/qcfinancial?logo=python&logoColor=white" alt="Python versions"/>
  <img src="https://img.shields.io/badge/platform-macOS%20%7C%20Linux%20%7C%20Windows-lightgrey" alt="Platform"/>
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus" alt="C++17"/>
  <img src="https://img.shields.io/github/license/qcfinancial/qcfinancial" alt="License"/>
  <a href="https://qcfinancial.github.io/qcfinancial-docs/">
    <img src="https://img.shields.io/badge/docs-GitHub%20Pages-blue?logo=jupyter&logoColor=white" alt="Docs"/>
  </a>
</p>

<h1 align="center">qcfinancial</h1>

<p align="center">
  A high-performance C++17 library for the valuation of linear interest rate and FX derivatives, exposed to Python via <a href="https://github.com/pybind/pybind11">pybind11</a>.<br/>
  Includes native support for Chilean market instruments (ICP-CLP, ICP-CLF/UF).
</p>

---

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [Supported Instruments](#supported-instruments)
- [Architecture](#architecture)
- [Building from Source](#building-from-source)
- [Platform Support](#platform-support)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [License](#license)

---

## Features

- **Cashflow types** — fixed rate, IBOR floating, overnight-compounded (SOFR, SONIA, ICP-CLP), and their multi-currency variants
- **Curve sensitivities** — first-order DV01 with respect to every node of a discounting or projection curve
- **Forward rates** — projection of floating rates from zero-coupon curves
- **FX derivatives** — FX forwards and multi-currency cashflow valuation from two discount curves
- **Chilean market** — native ICP-CLP and ICP-CLF (UF) cashflow types that implement the local overnight-index conventions
- **Python-first API** — every cashflow exposes a `record()` method returning a tuple ready for `pandas.DataFrame` construction, including `present_value` and `discount_factor`
- **Zero-copy performance** — C++17 core with pybind11 bindings; no Python overhead in the valuation loop

---

## Installation

```bash
pip install qcfinancial
```

Wheels are available on PyPI for **Python 3.10 – 3.13** on macOS and Linux (x86-64).

---

## Quick Start

```python
import qcfinancial as qf

# --- Build a fixed-rate leg ---
leg = qf.LegFactory.build_bullet_fixed_rate_leg(
    rec_pay       = qf.RecPay.Receive,
    start_date    = qf.QCDate(1, 1, 2024),
    end_date      = qf.QCDate(1, 1, 2026),
    bus_adj_rule  = qf.BusyAdjRules.No,
    settlement_periodicity = qf.Tenor("6M"),
    settlement_stub_period = qf.StubPeriod.No,
    settlement_calendar    = qf.BusinessCalendar(qf.QCDate(1, 1, 2024), 10),
    settlement_lag         = 2,
    notional               = 1_000_000,
    amort_is_cashflow      = False,
    interest_rate          = qf.QCInterestRate(0.05, qf.QCAct360(), qf.QCLinearWf()),
    currency               = qf.QCUSD(),
)

# --- Discount with a flat zero-coupon curve ---
curve  = qf.ZeroCouponCurve(...)
pv_eng = qf.PresentValue()
npv    = pv_eng.pv(valuation_date, leg, curve)
```

> Full worked examples are available in the [documentation](https://qcfinancial.github.io/qcfinancial-docs/).

---

## Supported Instruments

| Cashflow Type | Description |
|---|---|
| `SimpleCashflow` | Single deterministic payment |
| `SimpleMultiCurrencyCashflow` | Deterministic payment with FX conversion |
| `FixedRateCashflow` | Fixed rate interest period |
| `FixedRateMultiCurrencyCashflow` | Fixed rate with FX conversion |
| `IborCashflow` | Floating IBOR-style period |
| `IborMultiCurrencyCashflow` | IBOR with FX conversion |
| `IcpClpCashflow` | Chilean overnight index (ICP) in CLP |
| `IcpClfCashflow` | Chilean overnight index in CLF (UF) |
| `CompoundedOvernightRateCashflow2` | Generic compounded overnight rate (SOFR, SONIA, …) |
| `CompoundedOvernightRateMultiCurrencyCashflow2` | Compounded overnight with FX conversion |
| `OvernightIndexCashflow` | Overnight index with flexible settlement |
| `OvernightIndexMultiCurrencyCashflow` | Overnight index with FX conversion |

All types support `record()` for zero-copy `pandas.DataFrame` construction.

---

## Architecture

The library is organized in six layers, each depending only on layers below it.

```
Layer 6  ── Present Value / Pricing   (PresentValue, ForwardRates, ForwardFXRates)
Layer 5  ── Legs & Factory            (Leg, LegFactory)
Layer 4  ── Cashflows                 (SimpleCashflow, FixedRateCashflow, IborCashflow, …)
Layer 3  ── Curves                    (ZeroCouponCurve, interpolators)
Layer 2  ── Asset Classes             (QCInterestRate, QCCurrency, Tenor, indices)
Layer 1  ── Time                      (QCDate, QCBusinessCalendar)
```

The C++ core is compiled as a static library (`QC_DVE_CORE.a`) and linked into the Python extension module (`qcfinancial.cpython-<ver>-<platform>.so`).

---

## Building from Source

### Prerequisites

- CMake ≥ 3.15
- A C++17 compiler (Clang ≥ 14 or GCC ≥ 11)
- Python 3.10 – 3.13 with development headers
- Ninja (optional but recommended)

### Python wheel (recommended)

```bash
# Current Python version
python setup.py bdist_wheel

# Specific pyenv version
./compile.sh 3.12.1

# All supported versions (3.10, 3.11, 3.12, 3.13)
./compile.sh
```

Wheels land in `dist/`.

### CMake directly (IDE / debug builds)

```bash
# Configure
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release -GNinja

# Build
cmake --build cmake-build-release
```

Outputs:
- `cmake-build-release/QC_DVE_CORE.a` — static library
- `cmake-build-release/qcfinancial.cpython-<ver>-darwin.so` — Python extension

---

## Platform Support

| Platform | Architecture | Status |
|---|---|---|
| macOS 14+ | x86-64 | ✅ Supported |
| macOS 14+ | Apple Silicon (arm64) | ✅ Supported via `ARCHFLAGS` |
| Linux (RHEL 8.5+, Ubuntu) | x86-64 | ✅ Supported |
| Windows | x86-64 | ✅ Supported |

Linux builds use the Dockerfiles in `dockerfiles-toolchains/`:

```bash
# Example: build inside the Ubuntu container
docker build -f dockerfiles-toolchains/Dockerfile.cpp-env-ubuntu -t qcf-build .
docker run --rm -v $(pwd):/src qcf-build ./compile.sh
```

---

## Documentation

Tutorials and worked examples are hosted as Jupyter notebooks on GitHub Pages:

**[https://qcfinancial.github.io/qcfinancial-docs/](https://qcfinancial.github.io/qcfinancial-docs/)**

---

## Contributing

1. Fork the repository and create a feature branch off `develop`.
2. Follow the layer architecture — new cashflow types go in `include/cashflows/` and `source/cashflows/`.
3. Expose the new type in `source/qcf_binder.cpp` via a helper in `include/QcfinancialPybind11Helpers.h`.
4. Ensure `record()` ends with `present_value` and `discount_factor`.
5. Add tests in `Tests/` using the Catch2 framework.
6. Open a pull request against `develop`; `master` receives only stable releases.

See `CLAUDE.md` for the full adding-a-feature checklist and build instructions.

---

## License

Distributed under the MIT License. See [`LICENSE`](LICENSE) for details.

---

<p align="center">
  Made with ❤️ by <a href="https://github.com/qcfinancial">QCode</a>
</p>
