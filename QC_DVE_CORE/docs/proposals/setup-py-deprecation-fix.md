# Proposal: fix `setup.py` deprecation warning

Status: **drafted, not implemented**. Not yet decided whether to route through openspec.

## Root cause

The warning isn't about a missing `pyproject.toml` — it fires whenever `setup.py` is
invoked as a script (`python setup.py bdist_wheel`). Fix is to stop invoking it directly
and go through a proper PEP 517 frontend (`pip wheel` or `python -m build`) instead.
`setup.py` itself can mostly stay as-is: the `CMakeExtension`/`CMakeBuild` custom
`build_ext` is imperative logic that can't be expressed in static TOML, so a thin
`setup.py` remains necessary regardless of which frontend calls it.

## Call sites of `setup.py bdist_wheel` (surveyed 2026-07-24)

| Location | Usage |
|---|---|
| `compile.sh:45` | per-pyenv-version loop |
| `compile_one.sh:4` | single-version convenience script |
| `CMakeLists.txt:41,47` | two CLion custom targets, RHEL cross-compile (`--plat-name manylinux_2_34_x86_64` / `manylinux_2_28_x86_64`) |
| `docs/building.md:17`, `CLAUDE.md` | docs only |

No CI builds the wheel (`.github/workflows/docs.yml` only deploys MkDocs), so there's no
pipeline risk — this is purely a local/dev-machine change.

## Recommended fix (minimal, no new dependency)

1. **Add a minimal `pyproject.toml`** with just a `[build-system]` table:
   ```toml
   [build-system]
   requires = ["setuptools>=64", "wheel", "ninja"]
   build-backend = "setuptools.build_meta"
   ```
   `setup.py` is unchanged — it keeps `CMakeExtension`/`CMakeBuild`/`setup(...)` exactly
   as today.

2. **Swap the invocation** from `python setup.py bdist_wheel` to
   `pip wheel . --no-build-isolation --no-deps -w dist`.
   - `--no-build-isolation` keeps today's model: setuptools/wheel/ninja must already be
     installed in the target interpreter (already documented in CLAUDE.md's build
     prerequisites — no new package needed).
   - `pip` is already a hard dependency of the current workflow (used to
     `pip install setuptools wheel ninja`), so this needs zero new tooling.
   - `python -m build --wheel --no-isolation` is the alternative the warning itself
     points to, but it requires installing the extra `build` package per pyenv version
     for no functional benefit here — `pip wheel` is the lazier, equivalent choice.

3. **Update the four call sites**: `compile.sh`, `compile_one.sh`, the two
   `CMakeLists.txt` custom targets, and `docs/building.md`/`CLAUDE.md` build-command docs.

## Risk / test item

The two RHEL `CMakeLists.txt` targets pass `--plat-name` and `--dist-dir` straight to
`bdist_wheel`. Under `pip wheel`, those become PEP 517 config-settings:
`-C--build-option="--plat-name manylinux_2_34_x86_64"` — this pass-through needs a
one-time smoke test before relying on it (documented setuptools behavior, not yet run in
this repo). Env-var-based config (`CMAKE_ARGS`, `DEBUG`, `Python_ROOT_DIR`) is unaffected
either way since `setup.py` reads those from `os.environ`, not from argv.

Also worth noting: **`.gitmodules` isn't tracked** even though `pybind11`/`eigen`/`autodiff`
are gitlinks in the tree — they only work today because they're already populated on
disk. This doesn't block the minimal fix above (no sdist round-trip involved), but it
would block a future move to `python -m build` with sdist-based builds, or to CI-built
wheels. Separate issue, flagged for awareness only.

## Out of scope (optional follow-up, not needed to kill the warning)

Moving static metadata (`name`, `version`, `author`, `python_requires`,
`extras_require`) into `pyproject.toml [project]` is the "fuller" modernization, but it
touches the versioning convention CLAUDE.md documents (version lives in `setup.py` +
CLAUDE.md + `qcf_binder.cpp`). Not needed for this fix — leave as a later decision.

## Assessment

Small, mechanical, low-risk change (1 new file + edits to 4 existing call sites), no CI
impact. Fine to implement directly rather than routing through openspec, but that's an
open decision.
