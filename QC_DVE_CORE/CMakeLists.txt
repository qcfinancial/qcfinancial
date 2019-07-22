cmake_minimum_required(VERSION 3.13)
project(QC_DVE_CORE)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_library(QC_DVE_CORE STATIC "")
add_library(QC_DVE_PYBIND SHARED "")
add_library(QC_Financial SHARED "")
add_library(QC_Financial_3 SHARED "")
add_executable(QC_DVE_CORE_TESTS)

add_subdirectory(source)
add_subdirectory(Tests)

if (APPLE)
    target_compile_options(QC_DVE_CORE PRIVATE -fPIC)
    set(PYTHON2_LIB /System/Library/Frameworks/Python.framework/Versions/Current/lib/)
    set(PYTHON3_LIB /usr/local/Cellar/python/3.7.3/Frameworks/Python.framework/Versions/3.7/lib/)
    set(BOOST_PYTHON2_LIB /usr/local/Cellar/boost-python/1.70.0/lib/)
    set(BOOST_PYTHON3_LIB /usr/local/Cellar/boost-python3/1.70.0_1/lib/)
elseif(WIN32)
    target_compile_options(QC_Financial PRIVATE /bigobj)
    set(PYTHON2_LIB C:/Users/Soporte/AppData/Local/Programs/Python/Python27-16/libs/)
    set(PYTHON3_LIB C:/Users/Soporte/AppData/Local/Programs/Python/Python37/libs/)
    set(BOOST_PYTHON_LIB C:/boost_1_70_0/stage/lib/)
else()
    target_compile_options(QC_DVE_CORE PRIVATE -fPIC)
    set(PYTHON2_LIB /usr/lib64/)
    set(PYTHON3_LIB /usr/local/lib/)
    set(BOOST_PYTHON_LIB /usr/lib/)
endif()

if (APPLE)
    target_link_libraries(QC_DVE_PYBIND
            ${PYTHON2_LIB}libpython2.7.dylib)

    target_link_libraries(QC_Financial
            ${PYTHON2_LIB}libpython2.7.dylib
            ${BOOST_PYTHON2_LIB}libboost_python27-mt.dylib )

    target_link_libraries(QC_Financial_3
            ${PYTHON3_LIB}libpython3.7.dylib
            ${BOOST_PYTHON3_LIB}libboost_python37-mt.dylib)

elseif(WIN32)

    target_link_libraries(QC_DVE_PYBIND
            ${PYTHON2_LIB}python27.dll)

    target_link_libraries(QC_Financial
            ${PYTHON2_LIB}python27.dll
            ${BOOST_PYTHON_LIB}libboost_python27-vc120-mt-x64-1_70.lib )

    target_link_libraries(QC_Financial_3
<<<<<<< HEAD
            ${PYTHON3_LIB}libpython37.lib
            ${BOOST_PYTHON3_LIB}libboost_python37-vc120-mt-x64-1_70.lib)
=======
            ${PYTHON3_LIB}libpython3.7.dylib
            ${BOOST_PYTHON3_LIB}libboost_python37-mt.dylib)
>>>>>>> 1f93a8ee8e1b9a1f67184420cdcd40f4fab07c66

else()

    target_compile_options(QC_DVE_PYBIND PRIVATE -fPIC)
    target_link_libraries(QC_DVE_PYBIND
            ${PYTHON2_LIB}libpython2.7.so.1.0)

    target_link_libraries(QC_Financial
            ${PYTHON2_LIB}libpython2.7.so.1.0
            ${BOOST_PYTHON_LIB}libboost_python27.so)

    target_link_libraries(QC_Financial_3
            ${PYTHON3_LIB}libpython3.7m.so.1.0
            ${BOOST_PYTHON_LIB}libboost_python37.so)

endif()


target_link_libraries(QC_DVE_CORE_TESTS QC_DVE_CORE)
