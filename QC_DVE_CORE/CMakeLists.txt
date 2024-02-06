cmake_minimum_required(VERSION 3.27)
project(QC_DVE_CORE)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(-DPYTHON_EXECUTABLE=$(users/adiazv/.pyenv/versions/3.11.7/bin/))
add_library(QC_DVE_CORE STATIC "")
set_target_properties(QC_DVE_CORE PROPERTIES POSITION_INDEPENDENT_CODE ON)

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
