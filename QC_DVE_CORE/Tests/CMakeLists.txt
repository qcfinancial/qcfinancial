set(QC_INCLUDE ../include)

target_include_directories(QC_DVE_CORE_TESTS
        PRIVATE
        ${QC_INCLUDE}
        include
        )

target_sources(QC_DVE_CORE_TESTS
        PRIVATE
        CatchMain.cpp
        QCDateTests.cpp
        CompoundedOvernightRateCashflowTests.cpp
        QCCurrencyTests.cpp
        QCBusinessCalendarTests.cpp
        QCYearFractionTests.cpp
        QCWealthFactorTests.cpp
        QCInterestRateTests.cpp
        TenorTests.cpp
        FXRateTests.cpp
        InterestRateIndexTests.cpp
        TestHelpers.cpp
        CompoundedOvernightRateCashflowTests.cpp
        ForwardRatesTests.cpp
        PresentValueTests.cpp
        LegFactoryTests.cpp
        )
