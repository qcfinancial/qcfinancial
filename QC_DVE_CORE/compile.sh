#!/bin/bash
# Compile qcfinancial for one or all supported Python versions.
#
# Usage:
#   ./compile.sh                        # build all versions
#   ./compile.sh 3.12.1                 # build one version (positional arg)
#   PYTHON_VERSION=3.12.1 ./compile.sh  # build one version (env var)

set -euo pipefail

PYENV_ROOT="${HOME}/.pyenv"
AVAILABLE_VERSIONS=("3.10.13" "3.11.5" "3.12.1" "3.13.1")

build_for_version() {
    local version="$1"
    local python_root="${PYENV_ROOT}/versions/${version}"
    local python_bin="${python_root}/bin/python"

    if [[ ! -x "$python_bin" ]]; then
        echo "ERROR: Python ${version} not found at ${python_root}" >&2
        return 1
    fi

    echo ""
    echo "==> Building for Python ${version}"
    CMAKE_ARGS="-DPython_ROOT_DIR=${python_root} -DPython_EXECUTABLE=${python_bin}" \
        "${python_bin}" setup.py bdist_wheel
}

target="${1:-${PYTHON_VERSION:-}}"

if [[ -n "$target" ]]; then
    build_for_version "$target"
else
    for version in "${AVAILABLE_VERSIONS[@]}"; do
        build_for_version "$version"
    done
fi
