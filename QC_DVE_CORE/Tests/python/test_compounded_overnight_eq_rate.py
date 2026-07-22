"""Regression tests for CompoundedOvernightRateCashflow2 realized-coupon eq-rate state
(v1.12.0a4, change fix-compounded-overnight-eq-rate-state).

Root cause fixed: fixing() used to store the RAW daily-compounded product in _endDateWf,
so amount() — which inverts _endDateWf WITHOUT rounding (to stay differentiable) — recovered
the UNROUNDED equivalent rate and disagreed with settlementAmount()/interest(), which use the
rate rounded to _eqRateDecimalPlaces. The fix makes fixing() store wf(rounded eq-rate), so the
unrounded inversion in amount() recovers the rounded rate for a realized coupon, with no change
to the differentiable forward/projection path.

These tests FAIL on 1.12.0a3 and PASS on 1.12.0a4.

Task 2.4 (forward/projection path unchanged) is guaranteed structurally, not by a test here:
amount() and the ForwardRates code are unchanged, and fixing() is the only non-ForwardRates
writer of _endDateWf (it is realized-only — _getFixingWf throws if any fixing is missing). The
ForwardRates path (setRateCompoundedOvernightCashflow2) sets _endDateWf via setEndDateWf and
never calls fixing(), so it cannot be perturbed by this change.

Run: python test_compounded_overnight_eq_rate.py   (or pytest)
"""
import datetime
import math

import qcfinancial as qcf

# --- coupon parameters (a fully realized O/N coupon over 27-31 Dec 2021) ---
NOTIONAL = 10_000_000.0
AMORT = 100_000.0
AMORT_IS_CASHFLOW = True
SPREAD = 0.001
GEARING = 1.5
DP = 6  # small enough that round(eqRate, DP) != eqRate, so the pre-fix bug is material

ACC_START = (27, 12, 2021)
ACC_END = (31, 12, 2021)
PAY = (3, 1, 2022)

# daily realized fixings (decimal rates) for each fixing date
FIXINGS = {
    (27, 12, 2021): 0.0300,
    (28, 12, 2021): 0.0305,
    (29, 12, 2021): 0.0310,
    (30, 12, 2021): 0.0315,
}


def qcdate(dmy):
    return qcf.QCDate(*dmy)


def days(d1, d2):
    return (datetime.date(d2[2], d2[1], d2[0]) - datetime.date(d1[2], d1[1], d1[0])).days


def build_index():
    act360 = qcf.QCAct360()
    lin_wf = qcf.QCLinearWf()
    lin_act360 = qcf.QCInterestRate(0.0, act360, lin_wf)
    fixing_calendar = qcf.CalendarFactory.build(
        qcf.QCDate(1, 1, 2021), 6, [qcf.BusinessCalendarId.USNY]
    )
    return qcf.InterestRateIndex(
        "OITEST", lin_act360, qcf.Tenor("0d"), qcf.Tenor("1d"),
        fixing_calendar, fixing_calendar, qcf.QCUSD(),
    )


def build_cf():
    fixing_dates = qcf.DateList()
    for dmy in sorted(FIXINGS, key=lambda t: (t[2], t[1], t[0])):
        fixing_dates.append(qcdate(dmy))
    cf = qcf.CompoundedOvernightRateCashflow2(
        build_index(),
        qcdate(ACC_START), qcdate(ACC_END), qcdate(PAY),
        fixing_dates,
        NOTIONAL, AMORT, AMORT_IS_CASHFLOW, qcf.QCUSD(),
        SPREAD, GEARING,
        qcf.QCInterestRate(0.0, qcf.QCAct360(), qcf.QCLinearWf()),
        DP, 0, 0,
    )
    return cf


def make_ts():
    ts = qcf.time_series()
    for dmy, v in FIXINGS.items():
        ts[qcdate(dmy)] = v
    return ts


def interest_component(cf):
    """amount() net of amortization, since amort_is_cashflow is True."""
    return cf.amount() - (AMORT if AMORT_IS_CASHFLOW else 0.0)


def test_amount_matches_interest_and_settlement():  # task 2.1
    cf = build_cf()
    ts = make_ts()
    cf.set_fixings(ts)

    eq = cf.fixing(ts)                 # rounded equivalent rate (the "correct" rate)
    yf_acc = days(ACC_START, ACC_END) / 360.0
    manual = NOTIONAL * (eq * GEARING + SPREAD) * yf_acc

    # amount() (net amort) must equal interest(ts) exactly, and the manual rounded formula.
    assert abs(interest_component(cf) - cf.interest(ts)) < 1e-9, (
        interest_component(cf), cf.interest(ts))
    assert abs(interest_component(cf) - manual) < 1e-6, (interest_component(cf), manual)

    # settlement_amount() net amort matches amount() net amort up to USD minor-unit rounding.
    assert abs((cf.settlement_amount() - AMORT) - interest_component(cf)) <= 1.0


def test_amount_does_not_clobber_interest():  # task 2.2
    cf = build_cf()
    ts = make_ts()
    cf.set_fixings(ts)
    before = cf.interest(ts)
    cf.amount()                        # valuation call
    after = cf.interest(ts)            # must still be the rounded settlement interest
    assert abs(before - after) < 1e-12, (before, after)
    assert abs((cf.amount() - AMORT) - after) < 1e-9


def test_record_rate_value_and_cashflow():  # task 2.3
    cf = build_cf()
    ts = make_ts()
    cf.set_fixings(ts)
    rec = cf.record()
    eq = cf.fixing(ts)                 # rounded eq-rate
    assert abs(rec["rate_value"] - eq) < 1e-12, (rec["rate_value"], eq)
    yf_acc = days(ACC_START, ACC_END) / 360.0
    implied = NOTIONAL * (eq * GEARING + SPREAD) * yf_acc
    assert abs(rec["interest"] - implied) < 1e-6, (rec["interest"], implied)
    expected_cf = rec["interest"] + (AMORT if AMORT_IS_CASHFLOW else 0.0)
    assert abs(rec["cashflow"] - expected_cf) < 1e-6, (rec["cashflow"], expected_cf)


if __name__ == "__main__":
    for name, fn in sorted(globals().items()):
        if name.startswith("test_"):
            fn()
            print(f"ok  {name}")
    print("all tests passed")
