"""Spike: does QCBusinessCalendar being passed by value dominate leg-build cost?

Design: the leg spans 2024-2034 and always sees the SAME 120 in-range holidays,
so schedule/adjustment work is identical across every case. Set size is inflated
with padding holidays in 1900-2020, which the leg never looks up. Any time that
scales with total set size is therefore the std::set<QCDate> deep copy, not work.
"""
import sys
import time

import qcfinancial as qcf

N_LEGS = 5000
LEG_START = (15, 1, 2024)
LEG_END = (15, 1, 2034)


def make_calendar(n_padding):
    """120 realistic in-range holidays + n_padding holidays far in the past."""
    cal = qcf.BusinessCalendar(qcf.QCDate(1, 1, 1900), 200)
    # In-range: 12 scattered holidays per year, 2024-2034. Isolated days, so no
    # long runs for next_busy_day to walk.
    for y in range(2024, 2035):
        for m in range(1, 13):
            cal.add_holiday(qcf.QCDate(7, m, y))
    # Padding: far outside the leg's span, never looked up.
    added = 0
    y, m, d = 1900, 1, 1
    while added < n_padding:
        cal.add_holiday(qcf.QCDate(d, m, y))
        added += 1
        d += 7
        if d > 28:
            d = 1
            m += 1
            if m > 12:
                m = 1
                y += 1
    return cal


def build_n_legs(cal, n):
    start = qcf.QCDate(*LEG_START)
    end = qcf.QCDate(*LEG_END)
    tenor = qcf.Tenor("6M")
    rate = qcf.QCInterestRate(0.05, qcf.QCAct360(), qcf.QCLinearWf())
    clp = qcf.QCCLP()
    t0 = time.perf_counter()
    for _ in range(n):
        qcf.LegFactory.build_bullet_fixed_rate_leg(
            qcf.RecPay.RECEIVE, start, end, qcf.BusyAdjRules.FOLLOW, tenor,
            qcf.StubPeriod.NO, cal, 0, 1_000_000.0, True, rate, clp, False,
        )
    return time.perf_counter() - t0


def main():
    label = sys.argv[1] if len(sys.argv) > 1 else "run"
    print(f"=== {label} === {N_LEGS} legs, 10y semiannual, 120 in-range holidays")
    print(f"{'set size':>10} {'total_s':>10} {'us/leg':>10} {'vs 120':>10}", flush=True)
    base = None
    for n_pad in (0, 300, 900, 1900, 3900):
        cal = make_calendar(n_pad)
        build_n_legs(cal, 200)  # warm
        elapsed = build_n_legs(cal, N_LEGS)
        if base is None:
            base = elapsed
        print(
            f"{120 + n_pad:>10} {elapsed:>10.3f} {elapsed / N_LEGS * 1e6:>10.1f} "
            f"{elapsed / base:>9.2f}x",
            flush=True,
        )


if __name__ == "__main__":
    main()
