"""Tests for Operation / Portfolio batch state queries (v1.12.0).

Covers: row ordering by (op_key, leg_number) regardless of insertion order,
columnar output feeding polars without conversion, PV NaN conventions,
thread-count determinism, the (t1, t2] flow window, and error mapping.

Run: PYTHONPATH=<dir with qcfinancial .so> python test_portfolio_batch.py  (or pytest)
"""
import math

import numpy as np
import qcfinancial as qcf


def build_leg(notional=1_000_000.0, rate_value=0.05):
    rate = qcf.QCInterestRate(rate_value, qcf.QCAct360(), qcf.QCLinearWf())
    return qcf.LegFactory.build_bullet_fixed_rate_leg(
        rec_pay=qcf.RecPay.RECEIVE,
        start_date=qcf.QCDate(15, 1, 2026),
        end_date=qcf.QCDate(15, 1, 2029),
        bus_adj_rule=qcf.BusyAdjRules.MODFOLLOW,
        settlement_periodicity=qcf.Tenor("6M"),
        settlement_stub_period=qcf.StubPeriod.NO,
        settlement_calendar=qcf.BusinessCalendar(qcf.QCDate(1, 1, 2026), 10),
        settlement_lag=0,
        initial_notional=notional,
        amort_is_cashflow=True,
        interest_rate=rate,
        notional_currency=qcf.QCCLP(),
        is_bond=False,
    )


def build_operation(key, notional=1_000_000.0, rate_value=0.05):
    return qcf.Operation(
        key=key, legs=[build_leg(notional, rate_value)], rec_pay=[qcf.RecPay.RECEIVE]
    )


def flat_clp_curve(rate_value=0.05):
    return qcf.ZeroCouponCurve(
        qcf.QCLinearInterpolator(
            qcf.QCCurve(
                qcf.long_vec([1, 3650]), qcf.double_vec([rate_value, rate_value])
            )
        ),
        qcf.QCInterestRate(0.0, qcf.QCAct365(), qcf.QCCompoundWf()),
    )


T = qcf.QCDate(20, 3, 2026)


def test_row_order_independent_of_insertion_order():
    keys = [1003, 1001, 1002]
    port = qcf.Portfolio()
    for k in keys:
        port.add(build_operation(k))
    state = port.states_at(T)
    assert list(state["op_key"]) == [1001, 1002, 1003]
    assert list(state["leg_number"]) == [1, 1, 1]

    port2 = qcf.Portfolio()
    for k in sorted(keys):
        port2.add(build_operation(k))
    state2 = port2.states_at(T)
    for col in state:
        if col == "currency_legend":
            assert state[col] == state2[col]
        else:
            assert np.array_equal(state[col], state2[col], equal_nan=True), col


def test_feeds_polars_without_conversion():
    import polars as pl

    port = qcf.Portfolio()
    port.add(build_operation(1001))
    port.add(build_operation(1002, 500_000.0, 0.04))
    state = port.states_at(T, curves={"CLP": flat_clp_curve()})
    legend = state.pop("currency_legend")
    df = pl.DataFrame(state)
    assert df.height == 2
    assert df["op_key"].dtype == pl.Int64
    assert df["accrued_interest"].dtype == pl.Float64
    total_accrued = df["accrued_interest"].sum()
    assert total_accrued > 0.0
    assert legend == ["CLP"]


def test_pv_nan_conventions():
    port = qcf.Portfolio()
    port.add(build_operation(1001))
    # no curves -> NaN
    assert math.isnan(port.states_at(T)["present_value"][0])
    # curves without the leg's currency -> NaN, other columns still computed
    state = port.states_at(T, curves={"USD": flat_clp_curve()})
    assert math.isnan(state["present_value"][0])
    assert state["accrued_interest"][0] > 0.0
    # matching curve -> finite PV
    state = port.states_at(T, curves={"CLP": flat_clp_curve()})
    assert math.isfinite(state["present_value"][0])


def test_pv_matches_per_cashflow_discounting():
    curve = flat_clp_curve()
    leg = build_leg()
    port = qcf.Portfolio()
    port.add(qcf.Operation(key=1, legs=[leg], rec_pay=[qcf.RecPay.RECEIVE]))
    state = port.states_at(T, curves={"CLP": curve})
    expected = 0.0
    for i in range(leg.size()):
        cf = leg.get_cashflow_at(i)
        offset = T.day_diff(cf.date())
        if offset > 0:
            expected += cf.amount() * curve.get_discount_factor_at(offset)
    assert abs(state["present_value"][0] - expected) <= 1e-9 * abs(expected)


def test_thread_count_determinism():
    port = qcf.Portfolio()
    for k in range(1, 301):
        port.add(build_operation(k, 1000.0 * k, 0.03 + 0.0001 * k))
    curves = {"CLP": flat_clp_curve()}
    one = port.states_at(T, curves=curves, num_threads=1)
    many = port.states_at(T, curves=curves, num_threads=8)
    for col in one:
        if col == "currency_legend":
            assert one[col] == many[col]
        else:
            assert np.array_equal(one[col], many[col], equal_nan=True), col


def test_flow_window_half_open():
    port = qcf.Portfolio()
    port.add(build_operation(1001))
    state = port.states_at(T)
    first_serial = int(state["next_flow_date"][0])
    first_settlement = qcf.QCDate(first_serial)
    end = qcf.QCDate(15, 7, 2027)
    flows = port.flows_between(first_settlement, end)
    # flow at t1 excluded; the two following settlements (15-01-2027, 15-07-2027) included
    assert first_serial not in list(flows["settlement_date"])
    assert len(flows["op_key"]) == 2
    assert np.all(flows["total"] > 0.0)
    assert list(flows["interest"] + flows["amortization"]) == list(flows["total"])


def test_incremental_mutation_and_errors():
    port = qcf.Portfolio()
    port.add(build_operation(1001))
    port.add(build_operation(1002))
    port.remove(1001)
    assert list(port.states_at(T)["op_key"]) == [1002]
    try:
        port.add(build_operation(1002))
        raise AssertionError("duplicate add did not raise")
    except ValueError:
        pass
    try:
        port.remove(9999)
        raise AssertionError("missing remove did not raise")
    except ValueError:
        pass
    try:
        qcf.Operation(key=1, legs=[], rec_pay=[])
        raise AssertionError("empty operation did not raise")
    except ValueError:
        pass


def test_operation_accessors():
    op = build_operation(7)
    assert op.get_key() == 7
    assert op.number_of_legs() == 1
    assert op.get_rec_pay(0) == qcf.RecPay.RECEIVE
    assert op.get_leg(0).size() == build_leg().size()


if __name__ == "__main__":
    for name, fn in sorted(globals().items()):
        if name.startswith("test_"):
            fn()
            print(f"{name} ok")
    print("ALL OK")
