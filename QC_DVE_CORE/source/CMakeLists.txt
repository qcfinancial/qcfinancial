set(QC_INCLUDE ../include)
set(PYBIND11 ../pybind11/include)

if (APPLE)
    set(PYTHON2_H /System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7)
    set(PYTHON3_H /usr/local/Cellar/python@3.8/3.8.3/Frameworks/Python.framework/Versions/3.8/include/python3.8)
    set(BOOST_H /usr/local/Cellar/boost/1.75.0_1/include)

elseif(WIN32)
    set(BOOST_H_27 C:/boost_1_70_0)
    set(PYTHON2_H C:/Users/Soporte/AppData/Local/Programs/Python/Python27-16/include)
    set(PYTHON37_H C:/Users/Soporte/AppData/Local/Programs/Python/Python37/include)
    set(PYTHON38_H C:/Users/Soporte/AppData/Local/Programs/Python/Python38/include)
    set(PYTHON39_H C:/Users/Soporte/AppData/Local/Programs/Python/Python39/include)
    set(PYTHON310_H C:/Users/Soporte/AppData/Local/Programs/Python/Python310/include)
    set(BOOST_H C:/boost_1_79_0) # No se distingue por target, porque todas las versiones de boost
                                       # para las correspondientes versiones de Python están en la misma carpeta.

else()
    file(STRINGS /etc/os-release distro REGEX "^NAME=")
    string(REGEX REPLACE "NAME=\"(.*)\"" "\\1" distro "${distro}")
    if(${distro} STREQUAL "Ubuntu")
        set(PYTHON310_H /usr/include/python3.10)
        set(BOOST_H /usr/include/boost)
    else()
        set(PYTHON39_H /usr/include/python3.9)
        set(BOOST_H /usr/include/boost)
        set(PYTHON2_H /usr/include/python2.7)
        set(PYTHON36_H /usr/include/python3.6m)
    endif()
endif()


# --- QC_DVE_CORE ---------------------------------------------------------------------

target_include_directories(QC_DVE_CORE
        PRIVATE
        ${QC_INCLUDE})

target_sources(QC_DVE_CORE
        PRIVATE
        ChileanFixedRateBond.cpp
        FixedRateBond.cpp
        cashflows/SimpleCashflow.cpp
        cashflows/SimpleMultiCurrencyCashflow.cpp
        cashflows/FixedRateCashflow.cpp
        cashflows/FixedRateCashflow2.cpp
        cashflows/FixedRateMultiCurrencyCashflow.cpp
        cashflows/IborCashflow.cpp
        cashflows/IborCashflow2.cpp
        cashflows/IcpClfCashflow.cpp
        cashflows/IcpClpCashflow.cpp
        cashflows/IcpClpCashflow2.cpp
        cashflows/IcpClpCashflow3.cpp
        cashflows/CompoundedOvernightRateCashflow.cpp
        cashflows/IborMultiCurrencyCashflow.cpp
        asset_classes/InterestRateIndex.cpp
        Leg.cpp
        LegFactory.cpp
        asset_classes/QC30360.cpp
        asset_classes/QCAct30.cpp
        asset_classes/QCAct360.cpp
        asset_classes/QCAct365.cpp
        asset_classes/QCActAct.cpp
        asset_classes/QC3030.cpp
        time/QCBusinessCalendar.cpp
        QCClampedSpline.cpp
        QCCompoundWf.cpp
        QCContinousWf.cpp
        time/QCDate.cpp
        QCDiscountBondPayoff.cpp
        QCFactoryFunctions.cpp
        QCFixedRatePayoff.cpp
        QCFloatingRatePayoff.cpp
        QCFXForward.cpp
        QCHelperFunctions.cpp
        HelperFunctions.cpp
        QCIcpClfPayoff.cpp
        QCIcpClpPayoff.cpp
        QCInterestRate.cpp
        QCInterestRateBasisSwap.cpp
        QCInterestRateLeg.cpp
        QCInterestRatePayoff.cpp
        QCInterestRatePeriodsFactory.cpp
        QCInterpolator.cpp
        QCLinearInterpolator.cpp
        QCLinearWf.cpp
        QCLogLinearInterpolator.cpp
        QCProjectingInterestRateCurve.cpp
        QCTest.cpp
        QCTimeDepositPayoff.cpp
        QCWealthFactor.cpp
        QCZeroCouponCurve.cpp
        QCZeroCouponDiscountFactorCurve.cpp
        QCZeroCouponInterestRateCurve.cpp
        QCZeroCurveBootstrappingFromRates.cpp
        QCZeroCurveBootstrappingFromRatesAndFixedLegs.cpp
        QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve.cpp
        QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs.cpp
        QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs.cpp
        Tenor.cpp
        ZeroCouponCurve.cpp
        LegFactory.cpp)


# --- QC_DVE_PYBIND ---------------------------------------------------------------------

target_include_directories(QC_DVE_PYBIND
        PRIVATE
        ${QC_INCLUDE}
        ${PYTHON2_H})

target_sources(QC_DVE_PYBIND
        PRIVATE
        QC_DVE_PYBIND.cpp
        time/QCDate.cpp
        time/QCBusinessCalendar.cpp
        asset_classes/QC30360.cpp
        asset_classes/QCAct30.cpp
        asset_classes/QCAct360.cpp
        asset_classes/QCAct365.cpp
        asset_classes/QCActAct.cpp
        QCWealthFactor.cpp
        QCLinearWf.cpp
        QCCompoundWf.cpp
        QCContinousWf.cpp
        QCInterestRate.cpp
        QCInterestRatePayoff.cpp
        QCDiscountBondPayoff.cpp
        QCTimeDepositPayoff.cpp
        QCInterestRateLeg.cpp
        QCFixedRatePayoff.cpp
        QCFloatingRatePayoff.cpp
        QCIcpClfPayoff.cpp
        QCIcpClpPayoff.cpp
        QCInterpolator.cpp
        QCLinearInterpolator.cpp
        QCLogLinearInterpolator.cpp
        QCFXForward.cpp
        QCInterestRateBasisSwap.cpp
        QCInterestRatePeriodsFactory.cpp
        QCFactoryFunctions.cpp
        QCZeroCouponCurve.cpp
        QCZeroCouponInterestRateCurve.cpp
        QCProjectingInterestRateCurve.cpp
        QCZeroCouponDiscountFactorCurve.cpp
        QCHelperFunctions.cpp
        QCZeroCurveBootstrappingFromRates.cpp
        QCZeroCurveBootstrappingFromRatesAndFixedLegs.cpp
        QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve.cpp
        QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs.cpp
        QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs.cpp
        )

if (WIN32)
    set_target_properties(QC_DVE_PYBIND PROPERTIES SUFFIX ".pyd")
else()
    set_target_properties(QC_DVE_PYBIND PROPERTIES SUFFIX ".so")
endif()


# --- QC_Financial y QC_Financial_3 ---------------------------------------------------------------------

set(QC_FINANCIAL_SOURCES
        QC_Financial.cpp
        ChileanFixedRateBond.cpp
        FixedRateBond.cpp
        cashflows/SimpleCashflow.cpp
        cashflows/SimpleMultiCurrencyCashflow.cpp
        cashflows/FixedRateCashflow.cpp
        cashflows/FixedRateCashflow2.cpp
        cashflows/FixedRateMultiCurrencyCashflow.cpp
        cashflows/IborCashflow.cpp
        cashflows/IborCashflow2.cpp
        cashflows/IborMultiCurrencyCashflow.cpp
        cashflows/IcpClfCashflow.cpp
        cashflows/IcpClpCashflow.cpp
        cashflows/IcpClpCashflow2.cpp
        cashflows/IcpClpCashflow3.cpp
        cashflows/CompoundedOvernightRateCashflow.cpp
        asset_classes/InterestRateIndex.cpp
        Leg.cpp
        LegFactory.cpp
        HelperFunctions.cpp
        asset_classes/QC3030.cpp
        asset_classes/QC30360.cpp
        asset_classes/QCAct30.cpp
        asset_classes/QCAct360.cpp
        asset_classes/QCAct365.cpp
        asset_classes/QCActAct.cpp
        time/QCBusinessCalendar.cpp
        QCClampedSpline.cpp
        QCCompoundWf.cpp
        QCContinousWf.cpp
        time/QCDate.cpp
        QCDiscountBondPayoff.cpp
        QCFactoryFunctions.cpp
        QCFixedRatePayoff.cpp
        QCFloatingRatePayoff.cpp
        QCFXForward.cpp
        QCHelperFunctions.cpp
        QCIcpClfPayoff.cpp
        QCIcpClpPayoff.cpp
        QCInterestRate.cpp
        QCInterestRateBasisSwap.cpp
        QCInterestRateLeg.cpp
        QCInterestRatePayoff.cpp
        QCInterestRatePeriodsFactory.cpp
        QCInterpolator.cpp
        QCLinearInterpolator.cpp
        QCLinearWf.cpp
        QCLogLinearInterpolator.cpp
        QCProjectingInterestRateCurve.cpp
        QCTest.cpp
        QCTimeDepositPayoff.cpp
        QCWealthFactor.cpp
        QCZeroCouponCurve.cpp
        QCZeroCouponDiscountFactorCurve.cpp
        QCZeroCouponInterestRateCurve.cpp
        QCZeroCurveBootstrappingFromRates.cpp
        QCZeroCurveBootstrappingFromRatesAndFixedLegs.cpp
        QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve.cpp
        QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs.cpp
        QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs.cpp
        Tenor.cpp
        ZeroCouponCurve.cpp)

target_include_directories(QC_Financial
        PRIVATE
        ${QC_INCLUDE}
        ${BOOST_H_27}
        ${PYTHON2_H})

target_sources(QC_Financial
        PRIVATE
        ${QC_FINANCIAL_SOURCES})

target_include_directories(QC_Financial_36
        PRIVATE
        ${QC_INCLUDE}
        ${BOOST_H}
        ${PYTHON36_H})

target_sources(QC_Financial_36
        PRIVATE
        ${QC_FINANCIAL_SOURCES})

target_compile_definitions(QC_Financial_36
        PUBLIC
        PYTHON3)

target_include_directories(QC_Financial_37
        PRIVATE
        ${QC_INCLUDE}
        ${BOOST_H}
        ${PYTHON37_H})

target_sources(QC_Financial_37
        PRIVATE
        ${QC_FINANCIAL_SOURCES})

target_compile_definitions(QC_Financial_37
        PUBLIC
        PYTHON3)

target_include_directories(QC_Financial_38
        PRIVATE
        ${QC_INCLUDE}
        ${BOOST_H}
        ${PYTHON38_H})

target_sources(QC_Financial_38
        PRIVATE
        ${QC_FINANCIAL_SOURCES})

target_compile_definitions(QC_Financial_38
        PUBLIC
        PYTHON3)

target_include_directories(QC_Financial_39
        PRIVATE
        ${QC_INCLUDE}
        ${BOOST_H}
        ${PYTHON39_H})

target_sources(QC_Financial_39
        PRIVATE
        ${QC_FINANCIAL_SOURCES})

target_compile_definitions(QC_Financial_39
        PUBLIC
        PYTHON3)

target_include_directories(QC_Financial_310
        PRIVATE
        ${QC_INCLUDE}
        ${BOOST_H}
        ${PYTHON310_H})

target_sources(QC_Financial_310
        PRIVATE
        ${QC_FINANCIAL_SOURCES})

target_compile_definitions(QC_Financial_310
        PUBLIC
        PYTHON3)

if (WIN32)
    set_target_properties(QC_Financial PROPERTIES SUFFIX ".pyd")
    set_target_properties(QC_Financial_37 PROPERTIES SUFFIX ".pyd")
    set_target_properties(QC_Financial_38 PROPERTIES SUFFIX ".pyd")
    set_target_properties(QC_Financial_39 PROPERTIES SUFFIX ".pyd")
    set_target_properties(QC_Financial_310 PROPERTIES SUFFIX ".pyd")
else()
    set_target_properties(QC_Financial PROPERTIES SUFFIX ".so")
    set_target_properties(QC_Financial_36 PROPERTIES SUFFIX ".so")
    set_target_properties(QC_Financial_37 PROPERTIES SUFFIX ".so")
    set_target_properties(QC_Financial_38 PROPERTIES SUFFIX ".so")
    set_target_properties(QC_Financial_39 PROPERTIES SUFFIX ".so")
    set_target_properties(QC_Financial_310 PROPERTIES SUFFIX ".so")
endif()
