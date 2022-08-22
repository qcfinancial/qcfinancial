cmake_minimum_required(VERSION 3.13)
project(QC_DVE_CORE)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_library(QC_DVE_CORE STATIC "")
add_library(QC_DVE_PYBIND SHARED "")
add_library(QC_Financial SHARED "")
add_library(QC_Financial_36 SHARED "")
add_library(QC_Financial_37 SHARED "")
add_library(QC_Financial_38 SHARED "")
add_library(QC_Financial_39 SHARED "")
add_library(QC_Financial_310 SHARED "")
add_executable(QC_DVE_CORE_TESTS)
add_executable(manual_tests)
target_compile_definitions(QC_Financial_36 PUBLIC NOMBRE_MODULO=QC_Financial_36)
target_compile_definitions(QC_Financial_37 PUBLIC NOMBRE_MODULO=QC_Financial_37)
target_compile_definitions(QC_Financial_38 PUBLIC NOMBRE_MODULO=QC_Financial_38)
target_compile_definitions(QC_Financial_39 PUBLIC NOMBRE_MODULO=QC_Financial_39)
target_compile_definitions(QC_Financial_310 PUBLIC NOMBRE_MODULO=QC_Financial_310)

add_subdirectory(source)
add_subdirectory(Tests)
add_subdirectory(ManualTests)

if (APPLE)
    target_compile_options(QC_DVE_CORE PRIVATE -fPIC)
    set(PYTHON2_LIB /Users/alvarodiaz/opt/miniconda3/envs/py27/lib/)
    set(PYTHON3_LIB /usr/local/Cellar/python@3.8/3.8.3/Frameworks/Python.framework/Versions/3.8/lib/)
    set(BOOST_PYTHON2_LIB /usr/local/lib/)
    set(BOOST_PYTHON3_LIB /usr/local/lib/)

elseif(WIN32)
    target_compile_options(QC_Financial PRIVATE /bigobj)
    target_compile_options(QC_Financial_37 PRIVATE /bigobj)
    target_compile_options(QC_Financial_38 PRIVATE /bigobj)
    target_compile_options(QC_Financial_39 PRIVATE /bigobj)
    target_compile_options(QC_Financial_310 PRIVATE /bigobj)
    set(PYTHON2_LIB C:/Users/Soporte/AppData/Local/Programs/Python/Python27/libs/)
    set(PYTHON37_LIB D:/Users/adiazv/AppData/Local/Programs/Python/Python37/libs/)
    set(PYTHON38_LIB D:/Users/adiazv/AppData/Local/Programs/Python/Python38/libs/)
    set(PYTHON39_LIB D:/Users/adiazv/AppData/Local/Programs/Python/Python39/libs/)
    set(PYTHON310_LIB D:/Users/adiazv/AppData/Local/Programs/Python/Python310/libs/)
    set(BOOST_PYTHON2_LIB C:/boost_1_70_0/lib64-msvc-12.0/)
    set(BOOST_PYTHON3_LIB C:/local/boost_1_79_0/lib64-msvc-14.1/lib/)

else()
    file(STRINGS /etc/os-release distro REGEX "^NAME=")
    string(REGEX REPLACE "NAME=\"(.*)\"" "\\1" distro "${distro}")
    if(${distro} STREQUAL "Ubuntu")
        set(PYTHON310_LIB /usr/lib/x86_64-linux-gnu/)
        set(BOOST_PYTHON_LIB /usr/lib/x86_64-linux-gnu/)
    else()
        set(PYTHON39_LIB /usr/lib64/)
        set(PYTHON36_LIB /usr/lib64/)
        set(BOOST_PYTHON39_LIB /usr/lib64/)
        set(BOOST_PYTHON36_LIB /usr/lib64/)
    endif()
endif()

if (APPLE)
    target_link_libraries(QC_DVE_PYBIND
            ${PYTHON2_LIB}libpython2.7.dylib)

    target_link_libraries(QC_Financial
            ${PYTHON2_LIB}libpython2.7.dylib
            ${BOOST_PYTHON2_LIB}libboost_python27-mt.dylib )

    target_link_libraries(QC_Financial_39
            ${PYTHON3_LIB}libpython3.8.dylib
            ${BOOST_PYTHON3_LIB}libboost_python38.dylib)

elseif(WIN32)

    target_link_libraries(QC_DVE_PYBIND
            ${PYTHON2_LIB}python27.lib)

    target_link_libraries(QC_Financial
            ${PYTHON2_LIB}python27.lib
            ${BOOST_PYTHON2_LIB}libboost_python27-vc120-mt-x64-1_70.lib)

    target_link_libraries(QC_Financial_37
            ${PYTHON37_LIB}python37.lib
            ${BOOST_PYTHON3_LIB}libboost_python37-vc141-mt-x64-1_79.lib)

    target_link_libraries(QC_Financial_38
            ${PYTHON38_LIB}python38.lib
            ${BOOST_PYTHON3_LIB}libboost_python38-vc141-mt-x64-1_79.lib)

    target_link_libraries(QC_Financial_39
            ${PYTHON39_LIB}python39.lib
            ${BOOST_PYTHON3_LIB}libboost_python39-vc141-mt-x64-1_79.lib)

    target_link_libraries(QC_Financial_310
            ${PYTHON310_LIB}python310.lib
            ${BOOST_PYTHON3_LIB}libboost_python310-vc141-mt-x64-1_79.lib)
else()
    target_compile_options(QC_DVE_PYBIND PRIVATE -fPIC)
    target_link_libraries(QC_DVE_PYBIND
            ${PYTHON2_LIB}libpython2.7.so)

    target_link_libraries(QC_Financial
            ${PYTHON2_LIB}libpython2.7.so
            ${BOOST_PYTHON_LIB}libboost_python27.so)

    if (${distro} STREQUAL "Ubuntu")
        target_link_libraries(QC_Financial_310
                ${PYTHON310_LIB}libpython3.10.so.1.0
                ${BOOST_PYTHON_LIB}libboost_python310.so.1.74.0)
    else()
        target_link_libraries(QC_Financial_39
                ${PYTHON39_LIB}libpython3.9.so.1.0
                ${BOOST_PYTHON39_LIB}libboost_python39.so.1.75.0)

        target_link_libraries(QC_Financial_36
                ${PYTHON36_LIB}libpython3.6m.so.1.0
                ${BOOST_PYTHON36_LIB}libboost_python.so.1.53.0)
    endif ()

endif()

target_link_libraries(QC_DVE_CORE_TESTS QC_DVE_CORE)
