# Building from Source

## Prerequisites

| Tool | Minimum version |
|---|---|
| CMake | 3.15 |
| C++ compiler | Clang ≥ 14 or GCC ≥ 11 |
| Python | 3.10 – 3.13 (with dev headers) |
| Ninja | any (optional but recommended) |
| pyenv | any |

## Python Wheel (recommended)

```bash
# Build for the currently active pyenv version
python setup.py bdist_wheel

# Build for a specific version
./compile.sh 3.12.1

# Build for all supported versions (3.10, 3.11, 3.12, 3.13)
./compile.sh
```

Wheels are placed in `dist/`.

## CMake Directly (IDE / debug builds)

```bash
# Configure
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release -GNinja

# Build
cmake --build cmake-build-release
```

Outputs:

- `cmake-build-release/QC_DVE_CORE.a` — static library
- `cmake-build-release/qcfinancial.cpython-<ver>-darwin.so` — Python extension

## Development Environment

A `.venv` using Python 3.13 is provided for documentation and testing tooling.

```bash
# Create (one-time)
python3.13 -m venv .venv

# Activate
source .venv/bin/activate   # macOS / Linux
.venv\Scripts\activate      # Windows

# Install dev dependencies
pip install -r requirements-dev.txt
```

## Building the Documentation Locally

With the `.venv` activated and Doxygen installed:

```bash
mkdocs serve
```

Then open `http://127.0.0.1:8000` in your browser.

## Linux Builds (Docker)

Dockerfiles for RedHat and Ubuntu are in `dockerfiles-toolchains/`:

```bash
# Build the Docker image
docker build -f dockerfiles-toolchains/Dockerfile.cpp-env-ubuntu -t qcf-build .

# Run the compiler inside the container
docker run --rm -v $(pwd):/src qcf-build ./compile.sh
```

## macOS SDK Note

On macOS, `CMakeLists.txt` automatically detects the SDK path via `xcrun --show-sdk-path` to handle Xcode CLT changes that moved C++ stdlib headers. No manual intervention is needed.
