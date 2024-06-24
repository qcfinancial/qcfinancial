set(QC_INCLUDE ../include)
set(AUTODIFF ../autodiff)
set(EIGEN ../eigen)


target_include_directories(ad_wf
        PRIVATE
        ${QC_INCLUDE}
        ${AUTODIFF}
        ${EIGEN}
)

target_sources(ad_wf
        PRIVATE
        AD_wealth_factors.cpp
        ../include/asset_classes/ADWealthFactor.h
        ../include/asset_classes/ADLinearWf.h
        ../include/asset_classes/ADContinousWf.h
)

target_include_directories(ad_zcc
        PRIVATE
        ${QC_INCLUDE}
        ${AUTODIFF}
        ${EIGEN}
)

target_sources(ad_zcc
        PRIVATE
        AD_zero_coupon_curve.cpp
        ../include/asset_classes/ADInterestRate.h
        ../include/asset_classes/ADWealthFactor.h
        ../include/asset_classes/ADLinearWf.h
        ../include/asset_classes/ADContinousWf.h
        ../include/curves/ADScatter.h
        ../include/curves/ADInterpolator.h
        ../include/curves/ADLinearInterpolator.h
        ../include/asset_classes/ADZeroCouponCurve.h
        ../source/asset_classes/QCAct365.cpp
        ../source/time/QCDate.cpp
        ../source/time/QCBusinessCalendar.cpp
)