# Python examples and review artifacts

General home for Python examples and review artifacts against `qcfinancial` — a
single `uv` project, add new notebooks/scripts here as they come up rather than
spinning up a fresh subproject each time.

## Contents

- `settlement_currency_review.py` / `settlement_currency_review.xlsx` — from the
  `extend-settlement-ccy-pv-coverage` change: a marimo notebook and an Excel
  workbook, both exercising `ForwardFXRates`/`PresentValueFX` across all five
  multi-currency cashflow types (Fixed, Ibor, Overnight, CompoundedOvernight,
  Simple/NDF). See "What to check" below.

## Setup

This is a self-contained `uv` project, separate from the main package's
`setup.py`-based build. It depends on a locally-built `qcfinancial` wheel for
Python 3.14, which is **not** committed (matches the repo's existing convention
of not committing wheels — see `.gitignore`'s `dist/`).

1. Build the wheel for Python 3.14 from the repo root (`QC_DVE_CORE/`):

   ```bash
   uv python install 3.14
   uv venv --python 3.14 .venv-build314
   uv pip install --python .venv-build314/bin/python setuptools wheel ninja
   .venv-build314/bin/python setup.py bdist_wheel
   rm -rf .venv-build314
   ```

   This produces `dist/qcfinancial-<version>-cp314-cp314-<platform>.whl`. If the
   version in `pyproject.toml`'s `[tool.uv.sources]` doesn't match, update that
   path.

2. Sync this project:

   ```bash
   cd review
   uv sync
   ```

3. Run the notebook:

   ```bash
   uv run marimo edit settlement_currency_review.py    # interactive
   uv run marimo run settlement_currency_review.py      # read-only app
   ```

4. Regenerate the Excel workbook (only needed after editing
   `build_excel_review.py`):

   ```bash
   uv run python build_excel_review.py
   ```

## What to check

- `settlement_currency_review.py` — one section per cashflow type: spot fixing,
  CIP forward projection (floating and already-fixed), and settlement-currency
  PV with all derivative outputs. Each step's `expected` column is a closed-form
  value computed independently in the notebook itself.
- `settlement_currency_review.xlsx` — one worksheet per type. Rows 2-11 show the
  actual `qcfinancial` results as plain values (spot fixing, CIP forward PV
  floating/already-fixed, FX delta, node-5 curve derivatives) — read this first.
  Below that (from row 16) is the formula derivation: the same math as live
  Excel formulas (`MATCH`/`INDEX` for curve interpolation, then the CIP forward,
  chain-rule derivatives, and discounted PV), plus a cross-check block at the
  bottom comparing the formula-derived PV against the results rows at the top.
  `build_excel_review.py` computes the results rows by running the real
  `qcfinancial` extension live (via `import qcfinancial`) each time it's
  regenerated — not hand-copied numbers.

Both were verified end-to-end before being committed (notebook via
`marimo export`, workbook via a headless formula evaluator) — see
`openspec/changes/extend-settlement-ccy-pv-coverage/tasks.md` (tasks 9.2, 10.2)
for what was checked and how.
