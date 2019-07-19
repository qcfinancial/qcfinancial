set(QC_INCLUDE ../include)

target_include_directories(QC_DVE_CORE_TESTS
        PRIVATE
        ${QC_INCLUDE})

target_sources(QC_DVE_CORE_TESTS
        PRIVATE
        CatchMain.cpp
        QCDateTests.cpp)
