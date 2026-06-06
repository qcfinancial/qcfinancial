cmake_minimum_required(VERSION 3.20)
project(QC_DVE_CORE)

set(CMAKE_CXX_STANDARD 17)

# CLT 26 (macOS 26 SDK) moved C++ standard library headers into the SDK.
# The compiler's built-in search path no longer covers them, so we add it explicitly.
if(APPLE)
    execute_process(COMMAND xcrun --show-sdk-path OUTPUT_VARIABLE APPLE_SDK_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
    include_directories(SYSTEM "${APPLE_SDK_PATH}/usr/include/c++/v1")
endif()

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(QC_DVE_CORE STATIC ""
        include/present_value/ForwardFXRates.cpp
        include/present_value/ForwardFXRates.h
        include/present_value/FXRateEstimator.cpp
        include/present_value/FXRateEstimator.h)
set_target_properties(QC_DVE_CORE PROPERTIES POSITION_INDEPENDENT_CODE ON)

# set(Python_ROOT_DIR $ENV{HOME}/.pyenv/versions/3.10.16)
# set(Python_ROOT_DIR $ENV{HOME}/.pyenv/versions/3.11.11)
# set(Python_ROOT_DIR $ENV{HOME}/.pyenv/versions/3.12.1)
# set(Python_ROOT_DIR $ENV{HOME}/.pyenv/versions/3.13.1)
set(Python_FIND_STRATEGY LOCATION)
set(PYBIND11_FINDPYTHON ON)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
add_subdirectory(pybind11)
pybind11_add_module(qcfinancial source/qcf_binder.cpp)
target_link_libraries(qcfinancial PRIVATE QC_DVE_CORE)
target_include_directories(qcfinancial PRIVATE include)
# add_executable(QC_DVE_CORE_TESTS Tests/IcpClpCashflow3Tests.cpp)
# add_executable(manual_tests include/QcfinancialPybind11Helpers.h)

add_subdirectory(source)
add_subdirectory(Tests)
add_subdirectory(ManualTests)

add_custom_target(build_wheel
        COMMAND python3.11 ${CMAKE_SOURCE_DIR}/setup.py bdist_wheel --plat-name manylinux_2_34_x86_64 --dist-dir ${CMAKE_SOURCE_DIR}/dist
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Building Linux Python wheel"
)

if(APPLE)
set(PY312 "$ENV{HOME}/.pyenv/versions/3.12.8/bin/python3")
execute_process(
        COMMAND ${PY312} -c "import sysconfig; print(sysconfig.get_path('include'))"
        OUTPUT_VARIABLE PY312_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
        COMMAND ${PY312} -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))"
        OUTPUT_VARIABLE PY312_EXT_SUFFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
add_library(qcfinancial_py312 MODULE source/qcf_binder.cpp)
target_link_libraries(qcfinancial_py312 PRIVATE QC_DVE_CORE pybind11::headers)
target_include_directories(qcfinancial_py312 PRIVATE include ${PY312_INCLUDE_DIR})
target_link_options(qcfinancial_py312 PRIVATE -undefined dynamic_lookup)
set_target_properties(qcfinancial_py312 PROPERTIES
        PREFIX ""
        SUFFIX "${PY312_EXT_SUFFIX}"
        OUTPUT_NAME "qcfinancial"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/py312"
)
endif()

# target_link_libraries(QC_DVE_CORE_TESTS QC_DVE_CORE)