set(QC_INCLUDE ../include)
set(QC_DEPRECATED ../deprecated)

#[[
target_include_directories(QC_DVE_CORE_TESTS
        PRIVATE
        ${QC_INCLUDE}
        ${QC_DEPRECATED}
        include
        )
]]

#[[target_sources(QC_DVE_CORE_TESTS
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
        PresentValueFXTests.cpp
        LegFactoryTests.cpp
        )]]
