import marimo

__generated_with = "0.24.0"
app = marimo.App(width="medium")


@app.cell
def _():
    import marimo as mo
    import qcfinancial as qcf

    mo.md(f"qcfinancial loaded: `{qcf.id()}`")
    return mo, qcf


@app.cell
def _(mo):
    mo.md(r"""
    # Settlement-currency PV review

    Reviews `ForwardFXRates`/`PresentValueFX` across all five multi-currency
    cashflow types: `FixedRateMultiCurrencyCashflow`, `IborMultiCurrencyCashflow`,
    `OvernightIndexMultiCurrencyCashflow`, `CompoundedOvernightRateMultiCurrencyCashflow2`,
    and `SimpleMultiCurrencyCashflow` (the NDF-shaped type).

    For each type this notebook exercises, on a nominal of 1,000,000 USD settled
    in CLP:

    1. **Spot fixing** (`ForwardFXRates.set_fx_rate`) — historical fixing looked
       up from an `FXRateEstimator`.
    2. **CIP forward projection** (`ForwardFXRates.set_fx_rate_cip`) — before the
       fixing date (still floating) and after it (already fixed).
    3. **Settlement-currency PV** (`PresentValueFX.pv`) — PV plus all four
       derivative outputs: notional-curve, CIP-settlement-curve, discount-curve,
       and FX delta.

    Every type is built with a zero accrual rate (or, for the NDF, no accrual
    concept at all), so `amount()` in notional currency is exactly the nominal —
    this keeps the expected PV a closed-form `nominal * forward * discount_factor`
    that the Excel workbook (`settlement_currency_review.xlsx`) replicates
    independently for cross-checking.
    """)
    return


@app.cell
def _(qcf):
    # --- Shared market setup ---------------------------------------------------
    usd = qcf.QCUSD()
    clp = qcf.QCCLP()
    fx_rate = qcf.FXRate(usd, clp)  # USD strong, CLP weak
    one_day = qcf.Tenor("1D")
    fx_calendar = qcf.BusinessCalendar(qcf.QCDate(1, 1, 2021), 20)
    fx_index = qcf.FXRateIndex(fx_rate, "USDCLPOBS", one_day, one_day, fx_calendar)

    lin_act360 = qcf.QCInterestRate(0.0, qcf.QCAct360(), qcf.QCLinearWf())
    sofr_index = qcf.InterestRateIndex(
        "SOFRRATE", lin_act360, one_day, one_day, fx_calendar, fx_calendar, usd
    )

    NOMINAL = 1_000_000.0
    SPOT = 900.0
    START_DATE = qcf.QCDate(2, 1, 2024)
    END_DATE = qcf.QCDate(2, 1, 2025)
    FX_FIXING_DATE = qcf.QCDate(2, 7, 2024)
    VALUATION_FLOATING = qcf.QCDate(2, 1, 2024)  # before FX_FIXING_DATE
    VALUATION_FIXED = qcf.QCDate(3, 7, 2024)  # after FX_FIXING_DATE
    HISTORICAL_FIX = 875.0

    def build_curve(rates):
        plazos = qcf.long_vec()
        for p in (1, 30, 90, 180, 365, 730):
            plazos.append(p)
        rate_values = qcf.double_vec()
        for r in rates:
            rate_values.append(r)
        curve = qcf.QCCurve(plazos, rate_values)
        interp = qcf.QCLinearInterpolator(curve)
        return qcf.ZeroCouponCurve(interp, lin_act360)

    notional_curve = build_curve([0.01, 0.015, 0.02, 0.025, 0.03, 0.035])
    settlement_curve = build_curve([0.02, 0.025, 0.03, 0.035, 0.04, 0.045])

    fixing_ts = qcf.time_series()
    fixing_ts[FX_FIXING_DATE] = HISTORICAL_FIX
    fx_estimator = qcf.FXRateEstimator(fixing_ts, -1.0)
    return (
        END_DATE,
        FX_FIXING_DATE,
        HISTORICAL_FIX,
        NOMINAL,
        SPOT,
        START_DATE,
        VALUATION_FIXED,
        VALUATION_FLOATING,
        clp,
        fx_estimator,
        fx_index,
        lin_act360,
        notional_curve,
        settlement_curve,
        sofr_index,
        usd,
    )


@app.cell
def _(mo, qcf):
    def run_demo(name, make_cf, end_date, fx_fixing_date, val_floating, val_fixed,
                 notional_ccy_code, spot, notional_curve, settlement_curve, estimator,
                 historical_fix, nominal):
        rows = []

        # 1. Spot fixing.
        cf_for_fixing = make_cf()
        fwd = qcf.ForwardFXRates()
        fixed_cf = fwd.set_fx_rate(val_fixed, cf_for_fixing, estimator)
        rows.append({
            "step": "spot fixing",
            "valuation_date": str(val_fixed),
            "settlement_amount": fixed_cf.settlement_amount(),
            "expected": nominal * historical_fix if notional_ccy_code == "USD"
            else nominal / historical_fix,
        })

        # 2. CIP forward, still floating.
        cf_floating = make_cf()
        fwd2 = qcf.ForwardFXRates()
        projected_floating = fwd2.set_fx_rate_cip(
            val_floating, spot, cf_floating, notional_curve, settlement_curve)
        pvfx_floating = qcf.PresentValueFX()
        pv_floating = pvfx_floating.pv(val_floating, projected_floating, settlement_curve)
        t = val_floating.day_diff(end_date)
        df_notional = notional_curve.get_discount_factor_at(t)
        df_settlement = settlement_curve.get_discount_factor_at(t)
        forward = spot * df_notional / df_settlement
        expected_floating = (
            nominal * forward * df_settlement if notional_ccy_code == "USD"
            else nominal / forward * df_settlement
        )
        rows.append({
            "step": "CIP forward (floating)",
            "valuation_date": str(val_floating),
            "pv": pv_floating,
            "expected": expected_floating,
            "fx_delta": pvfx_floating.get_fx_delta(),
        })

        # 3. CIP forward, already fixed (valuation after the fixing date).
        cf_fixed_for_cip = make_cf()
        cf_fixed_for_cip.set_fx_rate_index_value(historical_fix)
        fwd3 = qcf.ForwardFXRates()
        projected_fixed = fwd3.set_fx_rate_cip(
            val_fixed, spot, cf_fixed_for_cip, notional_curve, settlement_curve)
        pvfx_fixed = qcf.PresentValueFX()
        pv_fixed = pvfx_fixed.pv(val_fixed, projected_fixed, settlement_curve)
        t_fixed = val_fixed.day_diff(end_date)
        df_settlement_fixed = settlement_curve.get_discount_factor_at(t_fixed)
        expected_fixed = (
            nominal * historical_fix * df_settlement_fixed if notional_ccy_code == "USD"
            else nominal / historical_fix * df_settlement_fixed
        )
        rows.append({
            "step": "CIP forward (already fixed)",
            "valuation_date": str(val_fixed),
            "pv": pv_fixed,
            "expected": expected_fixed,
            "fx_delta": pvfx_fixed.get_fx_delta(),
        })

        notional_derivs = pvfx_floating.get_notional_curve_derivatives()
        cip_derivs = pvfx_floating.get_cip_settlement_curve_derivatives()
        disc_derivs = pvfx_floating.get_discount_curve_derivatives()

        return mo.vstack([
            mo.md(f"### {name}"),
            mo.ui.table(rows, selection=None),
            mo.md(
                f"**Floating-case curve derivatives** (vertex index 4, ~365d): "
                f"notional=`{notional_derivs[4]:.6f}`, "
                f"cip_settlement=`{cip_derivs[4]:.6f}`, "
                f"discount=`{disc_derivs[4]:.6f}`, "
                f"sum(cip+discount)=`{cip_derivs[4] + disc_derivs[4]:.6f}` "
                f"(≈0 for strong-side notional: cancellation identity)"
            ),
        ])

    return (run_demo,)


@app.cell
def _(
    END_DATE,
    FX_FIXING_DATE,
    HISTORICAL_FIX,
    NOMINAL,
    SPOT,
    START_DATE,
    VALUATION_FIXED,
    VALUATION_FLOATING,
    clp,
    fx_estimator,
    fx_index,
    lin_act360,
    notional_curve,
    qcf,
    run_demo,
    settlement_curve,
    usd,
):
    def make_fixed_rate_cf():
        return qcf.FixedRateMultiCurrencyCashflow(
            START_DATE, END_DATE, END_DATE, NOMINAL, NOMINAL, True,
            lin_act360, usd, FX_FIXING_DATE, clp, fx_index, 1.0)

    fixed_rate_demo = run_demo(
        "FixedRateMultiCurrencyCashflow", make_fixed_rate_cf, END_DATE, FX_FIXING_DATE,
        VALUATION_FLOATING, VALUATION_FIXED, "USD", SPOT, notional_curve,
        settlement_curve, fx_estimator, HISTORICAL_FIX, NOMINAL)
    fixed_rate_demo
    return


@app.cell
def _(
    END_DATE,
    FX_FIXING_DATE,
    HISTORICAL_FIX,
    NOMINAL,
    SPOT,
    START_DATE,
    VALUATION_FIXED,
    VALUATION_FLOATING,
    clp,
    fx_estimator,
    fx_index,
    notional_curve,
    qcf,
    run_demo,
    settlement_curve,
    sofr_index,
    usd,
):
    def make_ibor_cf():
        cf = qcf.IborMultiCurrencyCashflow(
            sofr_index, START_DATE, END_DATE, START_DATE, END_DATE,
            NOMINAL, NOMINAL, True, usd, 0.0, 1.0,
            FX_FIXING_DATE, clp, fx_index, 1.0)
        cf.set_interest_rate_value(0.0)
        return cf

    ibor_demo = run_demo(
        "IborMultiCurrencyCashflow", make_ibor_cf, END_DATE, FX_FIXING_DATE,
        VALUATION_FLOATING, VALUATION_FIXED, "USD", SPOT, notional_curve,
        settlement_curve, fx_estimator, HISTORICAL_FIX, NOMINAL)
    ibor_demo
    return


@app.cell
def _(
    END_DATE,
    FX_FIXING_DATE,
    HISTORICAL_FIX,
    NOMINAL,
    SPOT,
    START_DATE,
    VALUATION_FIXED,
    VALUATION_FLOATING,
    clp,
    fx_estimator,
    fx_index,
    lin_act360,
    notional_curve,
    qcf,
    run_demo,
    settlement_curve,
    usd,
):
    def make_overnight_cf():
        return qcf.OvernightIndexMultiCurrencyCashflow(
            START_DATE, END_DATE, START_DATE, END_DATE, END_DATE, usd,
            NOMINAL, NOMINAL, True, 0.0, 1.0, lin_act360, "SOFRINDEX", 8,
            qcf.DatesForEquivalentRate.ACCRUAL, FX_FIXING_DATE, clp, fx_index)

    overnight_demo = run_demo(
        "OvernightIndexMultiCurrencyCashflow", make_overnight_cf, END_DATE,
        FX_FIXING_DATE, VALUATION_FLOATING, VALUATION_FIXED, "USD", SPOT,
        notional_curve, settlement_curve, fx_estimator, HISTORICAL_FIX, NOMINAL)
    overnight_demo
    return


@app.cell
def _(
    END_DATE,
    FX_FIXING_DATE,
    HISTORICAL_FIX,
    NOMINAL,
    SPOT,
    START_DATE,
    VALUATION_FIXED,
    VALUATION_FLOATING,
    clp,
    fx_estimator,
    fx_index,
    lin_act360,
    notional_curve,
    qcf,
    run_demo,
    settlement_curve,
    sofr_index,
    usd,
):
    def make_compounded_overnight_cf():
        return qcf.CompoundedOvernightRateMultiCurrencyCashflow2(
            sofr_index, START_DATE, END_DATE, END_DATE, qcf.DateList(),
            NOMINAL, NOMINAL, True, usd, 0.0, 1.0, lin_act360, 8, 0, 0,
            FX_FIXING_DATE, clp, fx_index)

    compounded_overnight_demo = run_demo(
        "CompoundedOvernightRateMultiCurrencyCashflow2", make_compounded_overnight_cf,
        END_DATE, FX_FIXING_DATE, VALUATION_FLOATING, VALUATION_FIXED, "USD", SPOT,
        notional_curve, settlement_curve, fx_estimator, HISTORICAL_FIX, NOMINAL)
    compounded_overnight_demo
    return


@app.cell
def _(
    END_DATE,
    FX_FIXING_DATE,
    HISTORICAL_FIX,
    NOMINAL,
    SPOT,
    VALUATION_FIXED,
    VALUATION_FLOATING,
    clp,
    fx_estimator,
    fx_index,
    notional_curve,
    qcf,
    run_demo,
    settlement_curve,
    usd,
):
    def make_simple_cf():
        return qcf.SimpleMultiCurrencyCashflow(
            END_DATE, NOMINAL, usd, FX_FIXING_DATE, clp, fx_index, 1.0)

    simple_demo = run_demo(
        "SimpleMultiCurrencyCashflow (NDF)", make_simple_cf, END_DATE, FX_FIXING_DATE,
        VALUATION_FLOATING, VALUATION_FIXED, "USD", SPOT, notional_curve,
        settlement_curve, fx_estimator, HISTORICAL_FIX, NOMINAL)
    simple_demo
    return


@app.cell
def _(mo):
    mo.md(r"""
    ## Reading this notebook

    - **`expected`** columns are closed-form, computed directly in this
      notebook's Python from the same formulas documented in
      `openspec/specs/settlement-currency-present-value/spec.md`, independent of
      the C++ implementation's internal derivative-caching path — agreement
      between `pv`/`settlement_amount` and `expected` is a first cross-check.
    - The **Excel workbook** (`settlement_currency_review.xlsx`, same directory)
      replicates the same formulas a third time, in spreadsheet form, over the
      same sample inputs — compare its cells against this notebook's `expected`
      and actual columns for the final cross-check.
    - The **cancellation identity** shown under each section (strong-side
      notional, same curve for CIP projection and discounting) is the scenario
      from `PresentValueFX`'s spec: `cip_settlement_derivative +
      discount_derivative ≈ 0` at every vertex, even though neither piece is
      individually zero.
    """)
    return


@app.cell
def _():
    return


if __name__ == "__main__":
    app.run()
