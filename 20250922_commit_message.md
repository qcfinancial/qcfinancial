chore(core): update pybind module and harmonize cashflow/curve APIs; bump version to 1.7.0

Summary
- Bumps Python module id() to version 1.7.0 with current build timestamp.
- Expands and refactors pybind11 bindings for dates, calendars, currencies, rates, curves, legs, and cashflows.
- Harmonizes cashflow interfaces (Fixed, Ibor, Overnight, Compounded, Simple, ICP/CLP) and improves consistency across headers/sources.
- Curves/Interpolator: minor API additions and const-correctness tweaks; small fixes in ZeroCouponCurve and QCInterpolator implementations.
- Interest rate utilities: small adjustments and cleanup.
- Leg: minor touch-ups aligning with updated cashflow interfaces.
- CMake: minor configuration adjustment.

Details
- QC_DVE_CORE/source/qcf_binder.cpp:
  - Large reformat and consolidation of bindings; improves readability and structure.
  - Updates exported id() to "version: 1.7.0, build: 2025-09-22 18:41".
  - Extends pickling and enum exposures; tightens type hints and argument names.
- Cashflows (headers and sources):
  - Aligns method names/signatures across FixedRateCashflow, IborCashflow, OvernightIndexCashflow, CompoundedOvernightRateCashflow (and *2 variants), SimpleCashflow, and IcpClpCashflow (and *2).
  - Adds minor helpers and clarifying comments; small bugfix/cleanup in implementations where needed.
- Curves/Interpolator:
  - QCInterpolator(.h/.cpp): small API/behavior improvements, const-correctness; minor additions.
  - ZeroCouponCurve(.h/.cpp): minor corrections and helper exposure.
  - QCCurve.h: adds utilities used by curve consumers and bindings.
- Rates/Leg:
  - QCInterestRate(.h/.cpp): small refactors/cleanups.
  - Leg(.h/.cpp): minor changes to integrate with the updated cashflow API surface. Add the method getCurrentCashflowIndex.
- Build system:
  - CMakeLists.txt: minor tweak to accommodate binding/headers changes.

Why
- Prepare a coherent 1.7.0 release of the Python bindings and core APIs.
- Improve consistency and developer ergonomics across cashflow components and curve utilities.

Notes
- No intentional breaking changes; existing APIs should continue to work.
- Verified compile locally via CI/IDE; no behavioral changes expected beyond added capabilities.

Files affected (staged)
- QC_DVE_CORE/CMakeLists.txt
- QC_DVE_CORE/include/Leg.h
- QC_DVE_CORE/include/QcfinancialPybind11Helpers.h
- QC_DVE_CORE/include/asset_classes/QCInterestRate.h
- QC_DVE_CORE/include/asset_classes/ZeroCouponCurve.h
- QC_DVE_CORE/include/cashflows/Cashflow.h
- QC_DVE_CORE/include/cashflows/CompoundedOvernightRateCashflow.h
- QC_DVE_CORE/include/cashflows/CompoundedOvernightRateCashflow2.h
- QC_DVE_CORE/include/cashflows/FixedRateCashflow.h
- QC_DVE_CORE/include/cashflows/FixedRateCashflow2.h
- QC_DVE_CORE/include/cashflows/IborCashflow.h
- QC_DVE_CORE/include/cashflows/IborCashflow2.h
- QC_DVE_CORE/include/cashflows/IcpClpCashflow.h
- QC_DVE_CORE/include/cashflows/IcpClpCashflow2.h
- QC_DVE_CORE/include/cashflows/OvernightIndexCashflow.h
- QC_DVE_CORE/include/cashflows/SimpleCashflow.h
- QC_DVE_CORE/include/curves/QCCurve.h
- QC_DVE_CORE/include/curves/QCInterpolator.h
- QC_DVE_CORE/source/Leg.cpp
- QC_DVE_CORE/source/QCInterestRate.cpp
- QC_DVE_CORE/source/QCInterpolator.cpp
- QC_DVE_CORE/source/ZeroCouponCurve.cpp
- QC_DVE_CORE/source/cashflows/CompoundedOvernightRateCashflow.cpp
- QC_DVE_CORE/source/cashflows/CompoundedOvernightRateCashflow2.cpp
- QC_DVE_CORE/source/cashflows/FixedRateCashflow.cpp
- QC_DVE_CORE/source/cashflows/FixedRateCashflow2.cpp
- QC_DVE_CORE/source/cashflows/IborCashflow.cpp
- QC_DVE_CORE/source/cashflows/IborCashflow2.cpp
- QC_DVE_CORE/source/cashflows/IcpClpCashflow.cpp
- QC_DVE_CORE/source/cashflows/IcpClpCashflow2.cpp
- QC_DVE_CORE/source/cashflows/OvernightIndexCashflow.cpp
- QC_DVE_CORE/source/cashflows/SimpleCashflow.cpp
- QC_DVE_CORE/source/qcf_binder.cpp
