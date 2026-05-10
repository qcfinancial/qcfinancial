# qcfinancial

**qcfinancial** is a high-performance C++17 library for the valuation of linear interest rate and FX derivatives, exposed to Python via [pybind11](https://github.com/pybind/pybind11). It includes native support for Chilean market instruments (ICP-CLP, ICP-CLF/UF).

## Highlights

- Fixed rate, IBOR, compounded overnight (SOFR, SONIA, ICP-CLP), and multi-currency cashflow types
- First-order DV01 sensitivities with respect to every node of a discounting or projection curve
- FX forwards and multi-currency valuation from two discount curves
- Native Chilean ICP-CLP and ICP-CLF (UF) instruments
- Every cashflow exposes `record()` — a tuple ready for `pandas.DataFrame` construction

## Installation

```bash
pip install qcfinancial
```

## Quick Start

```python
import qcfinancial as qf

leg = qf.LegFactory.build_bullet_fixed_rate_leg(
    rec_pay                = qf.RecPay.Receive,
    start_date             = qf.QCDate(1, 1, 2024),
    end_date               = qf.QCDate(1, 1, 2026),
    bus_adj_rule           = qf.BusyAdjRules.No,
    settlement_periodicity = qf.Tenor("6M"),
    settlement_stub_period = qf.StubPeriod.No,
    settlement_calendar    = qf.BusinessCalendar(qf.QCDate(1, 1, 2024), 10),
    settlement_lag         = 2,
    notional               = 1_000_000,
    amort_is_cashflow      = False,
    interest_rate          = qf.QCInterestRate(0.05, qf.QCAct360(), qf.QCLinearWf()),
    currency               = qf.QCUSD(),
)

pv_engine = qf.PresentValue()
npv = pv_engine.pv(valuation_date, leg, curve)
```

For full tutorials and worked examples see the [notebook documentation](https://qcfinancial.github.io/qcfinancial-docs/).
