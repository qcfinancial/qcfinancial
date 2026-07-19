"""Capture Leg.record() output across builders/stubs/rules as a golden file.

Used to verify that a change to date arithmetic or schedule generation is
behavior-preserving: capture before, capture after, diff. Any difference is a
defect, not an expected update.

    python golden_leg_record.py mine.json
    python -c "import json;a=json.load(open('golden_leg_record_1.12.0a2.json'));\
b=json.load(open('mine.json'));print([k for k in set(a)|set(b) if a.get(k)!=b.get(k)])"

`golden_leg_record_1.12.0a2.json` is the baseline captured at 590daf0, i.e.
*before* the 1.12.0a3 change that moved business-day adjustment from QCDate to
QCBusinessCalendar (commit 8b67db1, 180x faster leg building). That change
reproduced this file byte for byte. Regenerating this baseline would require
checking out 590daf0 and rebuilding, so treat it as a fixture.

KNOWN COVERAGE GAPS — these paths are not exercised here:
  - QCSettlementLagBehaviour.MOVE_TO_WORKING_DAY (only the DONT_MOVE default).
    This is the branch in QCDate::shift that distinguishes it from
    QCBusinessCalendar::shift; it deserves coverage most.
  - ICP-CLF legs, every multi-currency (mccy) variant, and
    compounded_overnight_rate_leg_2.
  - LONGFRONT2..LONGFRONT14 stubs (only plain LONGFRONT), and forBonds=True.
"""
import json
import sys

import qcfinancial as qcf

RULES = [
    ("FOLLOW", qcf.BusyAdjRules.FOLLOW),
    ("MODFOLLOW", qcf.BusyAdjRules.MODFOLLOW),
    ("PREVIOUS", qcf.BusyAdjRules.PREVIOUS),
    ("NO", qcf.BusyAdjRules.NO),
]
STUBS = [
    ("NO", qcf.StubPeriod.NO),
    ("SHORTFRONT", qcf.StubPeriod.SHORTFRONT),
    ("LONGFRONT", qcf.StubPeriod.LONGFRONT),
    ("SHORTBACK", qcf.StubPeriod.SHORTBACK),
    ("LONGBACK", qcf.StubPeriod.LONGBACK),
]


def calendar():
    """Realistic-ish calendar: scattered holidays 2020-2040, isolated days."""
    cal = qcf.BusinessCalendar(qcf.QCDate(1, 1, 2020), 25)
    for y in range(2020, 2041):
        for d, m in ((1, 1), (1, 5), (18, 9), (19, 9), (25, 12), (31, 12),
                     (7, 3), (14, 7), (2, 11), (8, 8)):
            cal.add_holiday(qcf.QCDate(d, m, y))
    return cal


def dump(leg):
    """record() where available; otherwise the dates that adjustment decides.

    The change under test only moves date arithmetic, so start/end/settlement
    dates are the fields that would actually shift if it misbehaved.
    """
    out = []
    for i in range(leg.size()):
        cf = leg.get_cashflow_at(i)
        if hasattr(cf, "record"):
            out.append([str(x) for x in cf.record()])
        else:
            row = [str(cf.date()), str(cf.amount()), str(cf.ccy())]
            for acc in ("get_start_date", "get_end_date", "get_settlement_date",
                        "get_start_date_icp", "get_end_date_icp", "get_nominal",
                        "get_amortization"):
                if hasattr(cf, acc):
                    row.append(f"{acc}={getattr(cf, acc)()}")
            out.append(row)
    return out


def main():
    cal = calendar()
    clp = qcf.QCCLP()
    usd = qcf.QCUSD()
    rate = qcf.QCInterestRate(0.0342, qcf.QCAct360(), qcf.QCLinearWf())
    idx_rate = qcf.QCInterestRate(0.0, qcf.QCAct360(), qcf.QCLinearWf())
    result = {}

    # Fixed rate: every rule x every stub, odd start/end to exercise stubs
    for rn, rule in RULES:
        for sn, stub in STUBS:
            for lag in (0, 2):
                key = f"fixed|{rn}|{sn}|lag{lag}"
                try:
                    leg = qcf.LegFactory.build_bullet_fixed_rate_leg(
                        qcf.RecPay.RECEIVE, qcf.QCDate(13, 2, 2024),
                        qcf.QCDate(20, 8, 2031), rule, qcf.Tenor("6M"), stub,
                        cal, lag, 1_000_000.0, True, rate, clp, False,
                    )
                    result[key] = dump(leg)
                except Exception as e:  # keep failures in the golden too
                    result[key] = f"ERROR: {type(e).__name__}: {e}"

    # Custom amortization
    amort = qcf.CustomNotionalAmort()
    amort.set_size(4)
    for i, (n, a) in enumerate([(1000.0, 250.0), (750.0, 250.0),
                                (500.0, 250.0), (250.0, 250.0)]):
        amort.set_notional_amort_at(i, n, a)
    for rn, rule in RULES:
        key = f"customamort|{rn}"
        try:
            leg = qcf.LegFactory.build_custom_amort_fixed_rate_leg(
                qcf.RecPay.PAY, qcf.QCDate(13, 2, 2024), qcf.QCDate(13, 2, 2026),
                rule, qcf.Tenor("6M"), qcf.StubPeriod.NO, cal, 1, amort, True,
                rate, clp,
            )
            result[key] = dump(leg)
        except Exception as e:
            result[key] = f"ERROR: {type(e).__name__}: {e}"

    # Ibor: exercises fixing calendar + fixing lag as well as settlement
    ibor_idx = qcf.InterestRateIndex(
        "TERM_SOFR_6M", idx_rate, qcf.Tenor("0d"), qcf.Tenor("6M"), cal, cal, usd)
    for rn, rule in RULES:
        for sn, stub in STUBS[:3]:
            key = f"ibor|{rn}|{sn}"
            try:
                leg = qcf.LegFactory.build_bullet_ibor_leg(
                    qcf.RecPay.RECEIVE, qcf.QCDate(13, 2, 2024),
                    qcf.QCDate(20, 8, 2029), rule, qcf.Tenor("6M"), stub, cal, 1,
                    qcf.Tenor("6M"), stub, cal, 2, ibor_idx, 1_000_000.0, True,
                    usd, 0.001, 1.0,
                )
                result[key] = dump(leg)
            except Exception as e:
                result[key] = f"ERROR: {type(e).__name__}: {e}"

    # Overnight index: separate fixing adjustment rule
    for rn, rule in RULES:
        key = f"onindex|{rn}"
        try:
            leg = qcf.LegFactory.build_bullet_overnight_index_leg(
                qcf.RecPay.RECEIVE, qcf.QCDate(13, 2, 2024), qcf.QCDate(20, 8, 2029),
                rule, qcf.BusyAdjRules.PREVIOUS, qcf.Tenor("3M"), qcf.StubPeriod.SHORTFRONT,
                cal, cal, 2, 1_000_000.0, True, 0.002, 1.0, idx_rate, "SOFRINDX", 8,
                usd, qcf.DatesForEquivalentRate.ACCRUAL,
            )
            result[key] = dump(leg)
        except Exception as e:
            result[key] = f"ERROR: {type(e).__name__}: {e}"

    # ICP-CLP
    for rn, rule in RULES:
        key = f"icpclp|{rn}"
        try:
            leg = qcf.LegFactory.build_bullet_icp_clp_leg(
                qcf.RecPay.RECEIVE, qcf.QCDate(13, 2, 2024), qcf.QCDate(20, 8, 2029),
                rule, qcf.Tenor("6M"), qcf.StubPeriod.SHORTFRONT, cal, 2,
                1_000_000.0, True, 0.005, 1.0,
            )
            result[key] = dump(leg)
        except Exception as e:
            result[key] = f"ERROR: {type(e).__name__}: {e}"

    # Direct date-arithmetic surface, independent of legs
    holidays = cal.get_holidays()
    dates = [qcf.QCDate(d, m, 2024) for d, m in
             ((1, 1), (2, 1), (17, 9), (18, 9), (19, 9), (20, 9), (30, 4), (31, 12))]
    result["business_day"] = [
        [str(d), rn, str(d.business_day(holidays, rule))]
        for d in dates for rn, rule in RULES
    ]

    json.dump(result, open(sys.argv[1], "w"), indent=1, sort_keys=True)
    n = sum(len(v) for v in result.values() if isinstance(v, list))
    errs = sum(1 for v in result.values() if isinstance(v, str))
    print(f"wrote {sys.argv[1]}: {len(result)} cases, {n} rows, {errs} error-cases")


if __name__ == "__main__":
    main()
