release(qcfinancial): v1.7.1 — update build config and Python bindings

- Build/CMake:
  - Point Python_ROOT_DIR to Python 3.11.11 (kept 3.10/3.12/3.13 as commented options).
  - Ensure QC_DVE_CORE builds as PIC and expose present_value sources in the target list.
  - Add pybind11 subdirectory and build the `qcfinancial` module from `source/qcf_binder.cpp`.
  - Link `qcfinancial` against `QC_DVE_CORE` and include project headers.
  - Keep Tests and ManualTests subdirectories wired up.

- Python packaging (setup.py):
  - Keep version in sync at 1.7.1.
  - Build the extension via CMake with explicit PYTHON_EXECUTABLE and Ninja support when available.
  - Improve macOS SDK and architecture handling during wheel builds.

- Bindings (qcf_binder.cpp):
  - Update `id()` to report `version: 1.7.1` and the current build timestamp.
  - Include present value utilities (ForwardFXRates, FXRateEstimator) in the Python surface.
  - Maintain opaque STL bindings for vectors/maps used across the API.
  - Restore method `get_cashflows` in object `Leg`.

Notes:
- No public API breaking changes; Python module name and linkage remain the same.
- This prepares the project for consistent local builds with Python 3.11 while keeping other versions as easy toggles in CMake.
