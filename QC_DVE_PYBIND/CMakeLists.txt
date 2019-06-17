cmake_minimum_required(VERSION 3.13)
project(QC_DVE_PYBIND)

set(CMAKE_CXX_STANDARD 14)

link_directories(../QC_DVE_CORE/cmake-build-release)
link_directories(C:/ProgramData/Anaconda2/libs)
include_directories(.)
include_directories(Include)
include_directories(C:/ProgramData/Anaconda2/Include)
include_directories(../QC_DVE_CORE/include)

add_library(QC_DVE_PYBIND SHARED
        main.cpp)
set_target_properties(QC_DVE_PYBIND PROPERTIES SUFFIX ".pyd")
target_link_libraries(QC_DVE_PYBIND QC_DVE_CORE)
