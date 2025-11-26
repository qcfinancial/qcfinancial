cmake_minimum_required(VERSION 3.20)
project(QC_DVE_CORE)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(QC_DVE_CORE STATIC ""
        include/present_value/ForwardFXRates.cpp
        include/present_value/ForwardFXRates.h
        include/present_value/FXRateEstimator.cpp
        include/present_value/FXRateEstimator.h)
set_target_properties(QC_DVE_CORE PROPERTIES POSITION_INDEPENDENT_CODE ON)

# set(Python_ROOT_DIR /users/adiazv/.pyenv/versions/3.10.16)
set(Python_ROOT_DIR /users/adiazv/.pyenv/versions/3.11.11)
# set(Python_ROOT_DIR /users/adiazv/.pyenv/versions/3.12.8)
# set(Python_ROOT_DIR /users/adiazv/.pyenv/versions/3.13.1)
add_subdirectory(pybind11)
pybind11_add_module(qcfinancial source/qcf_binder.cpp)
target_link_libraries(qcfinancial PRIVATE QC_DVE_CORE)
target_include_directories(qcfinancial PRIVATE include)
# add_executable(QC_DVE_CORE_TESTS Tests/IcpClpCashflow3Tests.cpp)
# add_executable(manual_tests include/QcfinancialPybind11Helpers.h)

add_subdirectory(source)
add_subdirectory(Tests)
add_subdirectory(ManualTests)

# target_link_libraries(QC_DVE_CORE_TESTS QC_DVE_CORE)