#ifndef ADINTERESTRATE_H
#define ADINTERESTRATE_H

#include "asset_classes/QCYearFraction.h"
#include "asset_classes/ADWealthFactor.h"

namespace ad = autodiff;

namespace QCode::ADFinancial {
    class ADInterestRate {
    public:
        ADInterestRate(
                std::unique_ptr<QCYearFraction> yearFraction,
                std::unique_ptr<ADWealthFactor> wealthFactor) :
                _yf(std::move(yearFraction)),
                _wf(std::move(wealthFactor)) {};


        ad::real wfFromDays(ad::real rate, long days) {
            ad::real yf = _yf->yf(days);
            return _wf->wf(rate, yf);
        };


        ad::real wfFromDates(ad::real rate, QCDate &startDate, QCDate &endDate) {
            ad::real yf = _yf->yf(startDate, endDate);
            return _wf->wf(rate, yf);
        };


        ad::real getRateFromWf(ad::real wf, QCDate &startDate, QCDate &endDate) {
            ad::real yf = _yf->yf(startDate, endDate);
            return _wf->rate(wf, yf);
        };


        ad::real getRateFromWf(ad::real wf, long days) {
            ad::real yf = _yf->yf(days);
            return _wf->rate(wf, yf);
        };


        double yf(QCDate &startDate, QCDate &endDate) {
            return _yf->yf(startDate, endDate);
        };


        const unique_ptr<ADWealthFactor>& getWealthFactor() {
            return _wf;
        };


        const unique_ptr<QCYearFraction>& getYearFraction() {
            return _yf;
        };


        std::string description() {
            return _wf->description() + "_" + _yf->description();
        };


        ~ADInterestRate() = default;

    private:
        std::unique_ptr<QCYearFraction> _yf;

        std::unique_ptr<ADWealthFactor> _wf;

    };
}


#endif //ADINTERESTRATE_H
