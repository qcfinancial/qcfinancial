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

# target_link_libraries(QC_DVE_CORE_TESTS QC_DVE_CORE)