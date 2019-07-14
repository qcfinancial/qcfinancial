cmake_minimum_required(VERSION 3.13)
project(QC_DVE_CORE)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_library(QC_DVE_CORE STATIC
        source/ChileanFixedRateBond.cpp
        source/FixedRateBond.cpp
        source/FixedRateCashflow.cpp
        source/FixedRateMultiCurrencyCashflow.cpp
        source/IborCashflow.cpp
        source/IcpClfCashflow.cpp
        source/IcpClpCashflow.cpp
        source/InterestRateIndex.cpp
        source/Leg.cpp
        source/LegFactory.cpp
        source/QC30360.cpp
        source/QCAct30.cpp
        source/QCAct360.cpp
        source/QCAct365.cpp
        source/QCActAct.cpp
        source/QCBusinessCalendar.cpp
        source/QCClampedSpline.cpp
        source/QCCompoundWf.cpp
        source/QCContinousWf.cpp
        source/QCDate.cpp
        source/QCDiscountBondPayoff.cpp
        source/QCFactoryFunctions.cpp
        source/QCFixedRatePayoff.cpp
        source/QCFloatingRatePayoff.cpp
        source/QCFXForward.cpp
        source/QCHelperFunctions.cpp
        source/QCIcpClfPayoff.cpp
        source/QCIcpClpPayoff.cpp
        source/QCInterestRate.cpp
        source/QCInterestRateBasisSwap.cpp
        source/QCInterestRateLeg.cpp
        source/QCInterestRatePayoff.cpp
        source/QCInterestRatePeriodsFactory.cpp
        source/QCInterpolator.cpp
        source/QCLinearInterpolator.cpp
        source/QCLinearWf.cpp
        source/QCLogLinearInterpolator.cpp
        source/QCProjectingInterestRateCurve.cpp
        source/QCTest.cpp
        source/QCTimeDepositPayoff.cpp
        source/QCWealthFactor.cpp
        source/QCZeroCouponCurve.cpp
        source/QCZeroCouponDiscountFactorCurve.cpp
        source/QCZeroCouponInterestRateCurve.cpp
        source/QCZeroCurveBootstrappingFromRates.cpp
        source/QCZeroCurveBootstrappingFromRatesAndFixedLegs.cpp
        source/QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve.cpp
        source/QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs.cpp
        source/QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs.cpp
        source/SimpleCashflow.cpp
        source/SimpleMultiCurrencyCashflow.cpp
        source/Tenor.cpp
        source/ZeroCouponCurve.cpp)

add_executable(QC_DVE_CORE_TESTS
        Tests/CatchMain.cpp
        Tests/QCDateTests.cpp)

target_link_libraries(QC_DVE_CORE_TESTS QC_DVE_CORE)
