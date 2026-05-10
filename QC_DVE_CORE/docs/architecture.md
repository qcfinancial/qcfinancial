# Architecture

The library is organized in six layers. Each layer depends only on layers below it.

```
Layer 6  ── Present Value / Pricing   (PresentValue, ForwardRates, ForwardFXRates)
Layer 5  ── Legs & Factory            (Leg, LegFactory)
Layer 4  ── Cashflows                 (SimpleCashflow, FixedRateCashflow, IborCashflow, …)
Layer 3  ── Curves                    (ZeroCouponCurve, interpolators)
Layer 2  ── Asset Classes             (QCInterestRate, QCCurrency, Tenor, indices)
Layer 1  ── Time                      (QCDate, QCBusinessCalendar)
```

## Layer 1 — Time

Located in `include/time/` and `source/time/`.

- **`QCDate`** — date representation and arithmetic (day count, business day adjustment)
- **`QCBusinessCalendar`** — holiday calendars and business day roll conventions

## Layer 2 — Asset Classes

Located in `include/asset_classes/` and `source/asset_classes/`.

| Class | Description |
|---|---|
| `QCCurrency` | Currency base class and subclasses (CLP, USD, EUR, CLF/UF, …) |
| `QCYearFraction` | Year fraction base + `QCAct360`, `QCAct365`, `QCActAct`, `QC30360`, `QC3030`, `QCAct30` |
| `QCWealthFactor` | Compounding base + `QCLinearWf`, `QCCompoundWf`, `QCContinousWf` |
| `QCInterestRate` | Bundles a value, a year fraction, and a wealth factor |
| `Tenor` | Period representation (e.g. 6M, 1Y) |
| `InterestRateIndex` | Interest rate index (IBOR, overnight) |
| `InterestRateCurve` | Abstract curve interface |
| `ZeroCouponCurve` | Concrete curve with node derivatives for sensitivities |

## Layer 3 — Curves

Located in `include/curves/`.

- **`QCCurve<T>`** — generic curve template over a sorted knot vector
- **Interpolators**: `QCLinearInterpolator`, `QCLogLinearInterpolator`, `QCClampedSpline`
- **`QCZeroCouponDiscountFactorCurve`**, **`QCZeroCouponInterestRateCurve`**, **`QCProjectingInterestRateCurve`**

## Layer 4 — Cashflows

Located in `include/cashflows/` and `source/cashflows/`. See [Instruments](instruments.md) for the full table.

All cashflow types share the abstract `Cashflow` interface (`amount()`, `ccy()`, `date()`). Every type exposes `record()` returning a `std::tuple` of all fields, with `present_value` and `discount_factor` as the final two elements.

## Layer 5 — Legs and Factory

- **`Leg`** — ordered container of `shared_ptr<Cashflow>`, accessible by index
- **`LegFactory`** — static factory methods that build complete legs from schedule parameters

## Layer 6 — Present Value / Pricing

Located in `include/present_value/`.

- **`PresentValue`** — discounts a `Leg` against a `ZeroCouponCurve`; computes first-order DV01 w.r.t. each curve node
- **`ForwardRates`** — sets forward rates on `IborCashflow`, `IcpClfCashflow`, `CompoundedOvernightRateCashflow2`, `OvernightIndexCashflow`, and their multi-currency variants
- **`ForwardFXRates`** — FX forward estimation from two discount curves
- **`FXRateEstimator`** — spot FX + basis point adjustments

## Python Bindings

The C++ core is compiled as a static library (`QC_DVE_CORE.a`) and linked into the Python extension module (`qcfinancial.cpython-<ver>-<platform>.so`) via pybind11.

- `source/qcf_binder.cpp` — the main `PYBIND11_MODULE(qcfinancial, m)` entry point
- `include/QcfinancialPybind11Helpers.h` — per-class registration helpers and `show()` functions

## Submodule Dependencies

| Submodule | Purpose |
|---|---|
| `pybind11/` | Python/C++ binding library |
| `eigen/` | Linear algebra (curve bootstrapping) |
| `autodiff/` | Automatic differentiation (sensitivity calculations) |
