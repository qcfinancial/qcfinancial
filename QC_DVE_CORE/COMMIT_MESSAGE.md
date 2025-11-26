release(qcfinancial): v1.7.3 — version bump, Python id() stamp, OvernightIndex fixes

- Python packaging (setup.py):
  - Bump package version to 1.7.3 and keep long_description metadata consistent.
  - Prefer Ninja when available; fall back gracefully if not installed.
  - On macOS, honor ARCHFLAGS and auto-detect a valid SDK via `xcrun --show-sdk-path` for more reliable local builds.

- Python bindings (qcf_binder.cpp):
  - Add qcfinancial.id() returning a human-readable version/build stamp: `version: 1.7.3, build: 2025-10-16 11:52`. 
  - Keep opaque STL bindings available to Python.

- Cashflows (OvernightIndexCashflow.cpp / .h):
  - Respect DatesForEquivalentRate when choosing accrual vs index period for equivalent rate calculation.
  - Improve numerical stability/precision by temporarily increasing eqRateDecimalPlaces during amount() computation and restoring it afterward.
  - Clarify derivative handling: when start/end index derivative vector sizes mismatch, return zero derivatives instead of throwing/using undefined values.

Notes:
- No breaking API changes; Python module now reports version/build via id().
- Tag: 1.7.3 (2025-10-16).
