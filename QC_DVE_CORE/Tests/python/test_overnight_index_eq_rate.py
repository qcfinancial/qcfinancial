"""Regression tests for OvernightIndexCashflow eq-rate state bugs (v1.11.4).

Covers: gearing in amount(), qcIndex dates in record(), decimal-places
mutation, settlementAmount guards, and the consistency matrix across
accrued_interest / record / show / settlement_amount in both
DatesForEquivalentRate modes.

Run: python test_overnight_index_eq_rate.py  (or pytest)
"""
import datetime
import math

import qcfinancial as qcf

NOTIONAL = 1_000_000_000.0
AMORT = 100_000_000.0
SPREAD = 0.002
GEARING = 2.0
DP = 8
START_IDX = 10_000.0
END_IDX = 10_250.0

ACC_START = (15, 1, 2024)
ACC_END = (15, 7, 2024)
IDX_START = (17, 1, 2024)  # lagged, different period length than accrual
IDX_END = (16, 7, 2024)


def qcdate(dmy):
    return qcf.QCDate(*dmy)


def days(d1, d2):
    return (datetime.date(d2[2], d2[1], d2[0]) - datetime.date(d1[2], d1[1], d1[0])).days


def cpp_round(x, dp):
    # std::round: half away from zero (x > 0 here)
    return math.floor(x * 10**dp + 0.5) / 10**dp


def make_cf(mode, lagged=False, gearing=GEARING, acc_end=ACC_END):
    idx_start = IDX_START if lagged else ACC_START
    idx_end = IDX_END if lagged else acc_end
    cf = qcf.OvernightIndexCashflow(
        qcdate(ACC_START), qcdate(acc_end), qcdate(idx_start), qcdate(idx_end),
        qcdate((17, 7, 2024)),
        qcf.QCCLP(), NOTIONAL, AMORT, True, SPREAD, gearing,
        qcf.QCInterestRate(0.0, qcf.QCAct360(), qcf.QCLinearWf()),
        "OISTEST", DP, mode,
    )
    cf.set_start_date_index(START_IDX)
    cf.set_end_date_index(END_IDX)
    return cf


def expected(mode, lagged=False, gearing=GEARING, rounded=True):
    """Interest from first principles: eq-rate over the mode's period, wf over accrual."""
    if mode == qcf.DatesForEquivalentRate.ACCRUAL:
        yf_eq = days(ACC_START, ACC_END) / 360.0
    else:
        eq_start = IDX_START if lagged else ACC_START
        eq_end = IDX_END if lagged else ACC_END
        yf_eq = days(eq_start, eq_end) / 360.0
    eq = (END_IDX / START_IDX - 1.0) / yf_eq
    if rounded:
        eq = cpp_round(eq, DP)
    yf_acc = days(ACC_START, ACC_END) / 360.0
    return NOTIONAL * (eq * gearing + SPREAD) * yf_acc


def test_amount_applies_gearing():  # task 3.1
    cf = make_cf(qcf.DatesForEquivalentRate.ACCRUAL)
    exp_unrounded = expected(qcf.DatesForEquivalentRate.ACCRUAL, rounded=False)
    assert abs(cf.amount() - (AMORT + exp_unrounded)) < 1e-6
    # amount() and settlement_amount() differ only by eq-rate + CLP rounding
    assert abs(cf.amount() - cf.settlement_amount()) < 10.0


def test_record_qcindex_consistency():  # task 3.2
    cf = make_cf(qcf.DatesForEquivalentRate.INDEX, lagged=True)
    rec = cf.record()
    yf_acc = days(ACC_START, ACC_END) / 360.0
    implied = NOTIONAL * (rec["rate_value"] * GEARING + SPREAD) * yf_acc
    assert abs(rec["interest"] - implied) < 1e-6
    assert abs(rec["interest"] - expected(qcf.DatesForEquivalentRate.INDEX, lagged=True)) < 1e-6


def test_zero_length_and_state():  # task 3.3
    cf = make_cf(qcf.DatesForEquivalentRate.ACCRUAL, acc_end=ACC_START)
    sa = cf.settlement_amount()
    assert not math.isnan(sa) and not math.isinf(sa)
    assert abs(sa - AMORT) < 1e-9
    cf2 = make_cf(qcf.DatesForEquivalentRate.ACCRUAL)
    cf2.amount()
    assert cf2.get_eq_rate_decimal_places() == DP


def _matrix(cf, three_arg_index_date, exp):
    end = qcdate(ACC_END)
    idx_date = qcdate(three_arg_index_date)
    one_arg = cf.accrued_interest(end, END_IDX)
    three_arg = cf.accrued_interest(end, idx_date, END_IDX)
    rec_interest = cf.record()["interest"]
    ts = qcf.time_series()
    ts[idx_date] = END_IDX
    from_ts = cf.accrued_interest(end, ts)
    for v in (one_arg, three_arg, rec_interest, from_ts):
        assert abs(v - exp) < 1e-6, (v, exp)
    # settlement_amount and show()-interest carry CLP rounding of the total
    assert abs((cf.settlement_amount() - AMORT) - exp) <= 1.0
    assert abs(qcf.show(cf)[14] - exp) <= 1.0


def test_matrix_qcaccrual():  # task 3.4
    cf = make_cf(qcf.DatesForEquivalentRate.ACCRUAL)
    _matrix(cf, ACC_END, expected(qcf.DatesForEquivalentRate.ACCRUAL))


def test_matrix_qcindex_lagged():  # task 3.5
    cf = make_cf(qcf.DatesForEquivalentRate.INDEX, lagged=True)
    _matrix(cf, IDX_END, expected(qcf.DatesForEquivalentRate.INDEX, lagged=True))
    # mid-period: 1-arg keeps its single-date contract == 3-arg (d, d, v)
    d = qcdate((15, 4, 2024))
    mid_idx = 10_120.0
    assert abs(cf.accrued_interest(d, mid_idx) - cf.accrued_interest(d, d, mid_idx)) < 1e-9


if __name__ == "__main__":
    for name, fn in sorted(globals().items()):
        if name.startswith("test_"):
            fn()
            print(f"ok  {name}")
    print("all tests passed")
