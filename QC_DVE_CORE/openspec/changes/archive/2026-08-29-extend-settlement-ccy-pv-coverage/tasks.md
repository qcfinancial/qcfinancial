## 1. OvernightIndexMultiCurrencyCashflow: derivative-caching surface

- [x] 1.1 Add `setFxRateNotionalCurveDerivatives`, `setFxRateSettlementCurveDerivatives`, `setFxRateSpotDerivative` setters and `_fxRateNotionalCurveDerivatives`/`_fxRateSettlementCurveDerivatives`/`_fxRateSpotDerivative` fields to `include/cashflows/OvernightIndexMultiCurrencyCashflow.h`, mirroring `include/cashflows/FixedRateMultiCurrencyCashflow.h:100-104,265-274`; verify the header compiles standalone (`cmake --build cmake-build-release --target QC_DVE_CORE`)
- [x] 1.2 Add `getAmountNotionalCurveDerivatives`, `getAmountSettlementCurveDerivatives`, `getAmountFxDelta` getters and `_amountNotionalCurveDerivatives`/`_amountSettlementCurveDerivatives`/`_amountFxDelta` fields, mirroring `FixedRateMultiCurrencyCashflow.h:106-110,276-286`
- [x] 1.3 Extend `OvernightIndexMultiCurrencyCashflow::settlementCurrencyAmount()` in `source/cashflows/OvernightIndexMultiCurrencyCashflow.cpp:149-155` to cache the strong/weak-side chain-rule derivatives, copying the logic in `source/cashflows/FixedRateMultiCurrencyCashflow.cpp:219-253` verbatim (swap in this type's own `_currency`/`_fxRateIndex`/notional-amount source); verify by unit test (task 6.1) — also fixed a pre-existing double-rounding bug: settlementCurrencyAmount() previously rounded via ccy()->amount()/settlementCurrency->amount() inside settlementCurrencyInterest()/settlementCurrencyAmortization(), unlike every other type; now recomputes the notional-currency amount unrounded and converts once, matching the "cached without rounding" spec requirement (user-confirmed fix, see design note below)

## 2. CompoundedOvernightRateMultiCurrencyCashflow2: derivative-caching surface

- [x] 2.1 Add the same 3 setters + 3 fields to `include/cashflows/CompoundedOvernightRateMultiCurrencyCashflow2.h`, mirroring task 1.1
- [x] 2.2 Add the same 3 getters + 3 fields, mirroring task 1.2
- [x] 2.3 Extend `CompoundedOvernightRateMultiCurrencyCashflow2::settlementCurrencyAmount()` in `source/cashflows/CompoundedOvernightRateMultiCurrencyCashflow2.cpp:145-146` with the same derivative-caching logic, using `CompoundedOvernightRateCashflow2::amount()` as the notional-currency amount; verify by unit test (task 6.2)

## 3. SimpleMultiCurrencyCashflow: settlementCurrencyAmount() and derivative-caching surface

- [x] 3.1 Add `settlementCurrencyAmount()` to `include/cashflows/SimpleMultiCurrencyCashflow.h`/`source/cashflows/SimpleMultiCurrencyCashflow.cpp`, moving the strong/weak-side conversion currently inline in `settlementAmount()` (`source/cashflows/SimpleMultiCurrencyCashflow.cpp:27-36`) into the new method, unrounded, unchanged formula
- [x] 3.2 Refactor `settlementAmount()` to call `settlementCurrencyAmount()`; add a regression test asserting `settlementAmount()`'s numeric output is unchanged for at least one strong-side and one weak-side case versus its pre-refactor formula (satisfies design.md's "SimpleMultiCurrencyCashflow settlementAmount() is unchanged" scenario) — regression test tracked in task 6.3
- [x] 3.3 Add the same 3 setters + 3 getters + 6 fields as tasks 1.1/1.2, and extend `settlementCurrencyAmount()` with the same strong/weak-side chain-rule derivative caching, using `nominal()` as the notional-currency amount

## 4. Wire the three types into ForwardFXRates and PresentValueFX

- [x] 4.1 Add a `SimpleMultiCurrencyCashflow` branch to `ForwardFXRates::setFXRate` in `source/present_value/ForwardFXRates.cpp:9-40`, using `getFXRateIndexFixingDate()` as the fixing date (mirroring the existing `OvernightIndexMultiCurrencyCashflow` branch at lines 27-32)
- [x] 4.2 Add 3 new branches to `ForwardFXRates::setFXRateCIP` in `source/present_value/ForwardFXRates.cpp:90-110`, one each for `OvernightIndexMultiCurrencyCashflow`, `CompoundedOvernightRateMultiCurrencyCashflow2`, `SimpleMultiCurrencyCashflow`, each calling the existing `_projectFXRateCIP<T>` template with that type's fixing-date getter
- [x] 4.3 Update the header comment at `include/present_value/ForwardFXRates.h:37-40` and `include/present_value/PresentValueFX.h:13` to list all five supported types instead of two
- [x] 4.4 Add 3 new branches to `PresentValueFX::pv` in `source/present_value/PresentValueFX.cpp:8-33`, one each for the three types, mirroring the existing `FixedRateMultiCurrencyCashflow`/`IborMultiCurrencyCashflow` branches exactly (same field reads, same `_notionalCurveDerivatives`/`_cipSettlementCurveDerivatives`/`_discountCurveDerivatives`/`_fxDelta` assignment)

## 5. Python bindings

- [x] 5.1 Bind the new setters/getters on `OvernightIndexMultiCurrencyCashflow` and `CompoundedOvernightRateMultiCurrencyCashflow2` in `source/qcf_binder.cpp` (near existing `settlement_currency_amount` bindings at lines ~1029 and ~1160), following the naming convention used for `FixedRateMultiCurrencyCashflow` (lines ~807-819) and `IborMultiCurrencyCashflow` (lines ~869-886)
- [x] 5.2 Bind `settlement_currency_amount` and the new setters/getters on `SimpleMultiCurrencyCashflow` (extending the `py::class_` block at `source/qcf_binder.cpp:745-761`)
- [x] 5.3 Verify bindings by building the wheel (`python setup.py bdist_wheel`) and importing `qcfinancial` in Python, confirming the new methods are present via `dir(...)` on each of the three classes — built `QC_DVE_CORE` and `qcfinancial` CMake targets directly via CLion's bundled cmake/ninja (system Python 3.9.6), both compiled clean; confirmed all new methods present via `dir()` and ran the full setFXRateCIP → PresentValueFX.pv pipeline end-to-end in Python for all three new types (Overnight, CompoundedOvernight, Simple/NDF) — PV matched the closed-form expected value to float precision in each case

## 6. C++ tests

- [x] 6.1 Add `OvernightIndexMultiCurrencyCashflow` scenarios to `Tests/PresentValueFXTests.cpp` covering: forward FX projection (still-floating and already-fixed), strong-side and weak-side `settlementCurrencyAmount()` derivatives, and `PresentValueFX::pv` (matured, strong-side cancellation, distinct CIP/discount curves, FX delta sign) — mirror the existing `FixedRateMultiCurrencyCashflow`/`IborMultiCurrencyCashflow` test cases — **now actually run**: the Tests target was re-enabled (see "Tests target re-enablement" note below) and `PresentValueFXTests.cpp` passes 16/16 test cases, 143/143 assertions
- [x] 6.2 Add the same scenario coverage for `CompoundedOvernightRateMultiCurrencyCashflow2` — covered by the same 16/16 passing run
- [x] 6.3 Add the same scenario coverage for `SimpleMultiCurrencyCashflow`, plus the spot-fixing `setFXRate` scenario from the new "Spot-fixing FX rate assignment covers SimpleMultiCurrencyCashflow" requirement and the `settlementAmount()`-unchanged regression test from task 3.2 — covered by the same 16/16 passing run; also fixed two of my own bugs found only by actually compiling: a misqualified `DatesForEquivalentRate` enum reference, and a pre-existing `QCCurve` const-correctness bug in the file's own `buildCurveWithNodes` helper (unrelated to this change, but blocking)
- [x] 6.4 Confirm the existing "PresentValueFX: unsupported cashflow type throws" test (`Tests/PresentValueFXTests.cpp:260-277`, which uses `SimpleCashflow`) still passes unmodified — the three newly-supported types must not appear in any remaining "unsupported" assertion — confirmed by inspection: that test's `SimpleCashflow` case is untouched and no other test asserts these three types are unsupported

## 7. Versioning

- [x] 7.1 Bump version to `1.14.0a3` in `setup.py`, the "Current version" line in `CLAUDE.md`, and the `id()` string in `source/qcf_binder.cpp`; verify all three agree by grepping for the version string across the three files

## 8. Python review environment (uv)

- [x] 8.1 Create `pyproject.toml` pinning Python 3.14, with `marimo`, `openpyxl` (or equivalent, if needed for any generation script), and the locally-built `qcfinancial` wheel as dependencies; run `uv sync` and verify a `.venv`/`uv.lock` are created in-project — created `review/pyproject.toml` (`review/` is the in-project sub-environment); `uv` itself had to be installed first (`brew install uv`, user-approved) and `uv python install 3.14` fetched CPython 3.14.7; `uv sync` created `review/.venv` + `review/uv.lock`
- [x] 8.2 Build the `qcfinancial` wheel for Python 3.14 (`./compile.sh 3.14.5` or `uv run python setup.py bdist_wheel`) and install it into the uv environment; verify `uv run python -c "import qcfinancial"` succeeds — built via a throwaway `uv venv --python 3.14` with setuptools/wheel/ninja installed and CLion's bundled cmake/ninja on PATH (system has neither); produced `dist/qcfinancial-1.14.0a3-cp314-cp314-macosx_26_0_arm64.whl`, referenced from `review/pyproject.toml` via `[tool.uv.sources]`; `uv run python -c "import qcfinancial"` in `review/` confirms `qcf.id()` reports 1.14.0a3

## 9. Marimo notebook (review artifact)

- [x] 9.1 Build a marimo notebook with one section per multi-currency type (Fixed, Ibor, Overnight, CompoundedOvernight, Simple/NDF) demonstrating: constructing a sample cashflow, `ForwardFXRates::setFXRate` (spot-fixing) against a sample `FXRateEstimator`, `ForwardFXRates::setFXRateCIP` (forward projection) against two sample curves, and `PresentValueFX::pv` with all four derivative outputs (notional-curve, CIP-settlement-curve, discount-curve, FX delta) printed/tabulated — `review/settlement_currency_review.py`; shared `run_demo()` helper runs all 3 steps per type against a common curve/spot setup, closed-form `expected` computed independently alongside each actual value
- [x] 9.2 Verify the notebook runs end-to-end under `uv run marimo run <notebook>.py` (or edit mode) with no errors, using the wheel built in task 8.2 — `uv run marimo check` clean (after `--fix` for 2 cosmetic dedent warnings); `uv run marimo export html` executed all 5 type-sections successfully — verified the actual computed PV (`873350642.3070815`) and fixed settlement amount (`875000000.0`) appear correctly in the rendered output for every type, no traceback

## 10. Excel workbook (review artifact)

- [x] 10.1 Build one worksheet per multi-currency type replicating, with formulas only (no VBA, no macros): the CIP forward formula (`Forward = Spot * DF_notional / DF_settlement`), its curve-vertex derivatives (finite-precision closed-form, not bump-and-reprice, per design.md), the strong/weak-side `settlementCurrencyAmount()` chain rule, and the discounted PV with its three derivative vectors and FX delta — `review/build_excel_review.py` generates `review/settlement_currency_review.xlsx`: a shared `Curves` sheet plus 5 per-type sheets, each replicating linear rate interpolation (`MATCH`/`INDEX`), `wf = 1 + rate*t/360`, `DF = 1/wf`, the CIP forward + per-node vertex derivatives, the strong-side amount chain rule, discounted PV, FX delta, and the cancellation-identity check, all as live cell formulas
- [x] 10.2 Cross-check each worksheet's output cells against the marimo notebook's output for the same sample inputs (same curves, spot, notional, dates); verify agreement to at least 6 significant figures and note any discrepancy before treating the workbook as validated — installed the `formulas` package (Python Excel-formula evaluator, temporarily; removed after) to recalculate the workbook headlessly: found and fixed a genuine off-by-one bug in the sheet's own column-mapping helper (data was landing one column left of the labeled node, colliding with the label column) — after the fix, every sheet's PV (both floating and already-fixed cases), all 6 per-node curve-vertex derivatives, and the FX delta match the qcfinancial reference values to full double-precision (16 significant figures), and the cancellation identity (`dPV/dCip + dPV/dDiscount ≈ 0`) holds exactly at every node

## 11. Documentation

- [x] 11.1 Update `CLAUDE.md`'s Layer 6 `PresentValueFX` description to list all five supported cashflow types instead of two

## 12. Tests target re-enablement (user follow-up, outside original scope)

The Tests target had been commented out (per `CLAUDE.md`) since before this change; the user asked to
re-enable it and actually run `PresentValueFXTests.cpp`. Doing so surfaced several pre-existing issues
unrelated to this change:

- [x] 12.1 Uncomment `Tests/CMakeLists.txt`'s `target_include_directories`/`target_sources` blocks; add
      `add_executable(QC_DVE_CORE_TESTS)` + `target_link_libraries(QC_DVE_CORE_TESTS QC_DVE_CORE)` to the
      root `CMakeLists.txt`, replacing the stale/incorrect commented-out lines that referenced a single
      nonexistent source file
- [x] 12.2 Fix vendored `include/catch/catch-2.hpp`: Catch2 v2's `CATCH_PLATFORM_MAC` branch hardcoded an
      x86 `int $3` inline-asm breakpoint trap, which doesn't assemble on arm64 — this had silently never
      been buildable on the project's primary (Apple Silicon) machine. Branch on `__arm64__`/`__aarch64__`
      to use `__builtin_debugtrap()` there, keep the original x86 asm on Intel
- [x] 12.3 Fix `include/present_value/ForwardRates.h`: missing includes for `IborCashflow`,
      `IborMultiCurrencyCashflow`, `CompoundedOvernightRateCashflow2`,
      `CompoundedOvernightRateMultiCurrencyCashflow2` (relied on being included after those headers by
      whichever translation unit used it — true for `qcf_binder.cpp`, false for test files including it
      directly)
- [x] 12.4 Fix two bugs of my own in `Tests/PresentValueFXTests.cpp`, found only once actually compiled: a
      misqualified `DatesForEquivalentRate` enum reference (it's namespace-scoped, not nested in
      `OvernightIndexCashflow`), and a pre-existing const-correctness bug in the file's own
      `buildCurveWithNodes` helper (`QCCurve`'s constructor takes non-const `vector&`; `plazos` was passed
      straight through from a `const&` parameter) that blocked the whole file, mine included
- [x] 12.5 Exclude 3 pre-existing, already-broken test files from the target rather than fix them (out of
      scope for this change): `ForwardRatesTests.cpp` and `PresentValueTests.cpp` reference
      `ForwardRates::setRateCompoundedOvernightCashflow` (removed/renamed) and a class `IcpClpCashflow3`
      that doesn't exist; `IcpClpCashflow3Tests.cpp` references the same missing class. Commented out in
      `Tests/CMakeLists.txt` with an explanatory note pointing here
- [x] 12.6 Link the deprecated `CompoundedOvernightRateCashflow` (tested by
      `CompoundedOvernightRateCashflowTests.cpp`, also used by `TestHelpers::getCashflow`/
      `getCashflowWithAmort`) directly into the test binary — `deprecated/*.cpp` was never part of any
      build target, so its implementation didn't exist anywhere in the link graph
- [x] 12.7 Build and run `QC_DVE_CORE_TESTS`: 85 test cases, 120895 assertions total; 78/85 cases pass.
      All 16 `PresentValueFXTests.cpp` cases pass (143/143 assertions) — this change's own tests are fully
      green. The 7 failures are pre-existing and unrelated to this change: 2 in
      `CompoundedOvernightRateCashflowTests.cpp` (assertion text/exception-type mismatches against the
      deprecated class), 4 in `QCBusinessCalendarTests.cpp` (holiday-list size mismatches), 1 in
      `FXRateTests.cpp` (a missing expected exception) — not investigated further, out of scope for this
      change

**Not fixed, left for a separate follow-up if wanted:** the 3 excluded test files (task 12.5) and the 7
pre-existing failures (task 12.7).
