#include "filter.h"

#include <cmath>

Filter::Filter(QObject *parent) : QObject(parent),
    cutoff(0.99),
    resonance(0.0),
    mode(FILTER_MODE_LOWPASS),
    buf0(0.0),
    buf1(0.0),
    buf2(0.0),
    buf3(0.0),
    cutoffMod(0.0)
{
    calculateFeedbackAmount();
}

// By Paul Kellett
// http://www.musicdsp.org/showone.php?id=29

double Filter::process(double inputValue) {
    if (inputValue == 0.0) return inputValue;
    double calculatedCutoff = getCalculatedCutoff();
    buf0 += calculatedCutoff * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));
    buf1 += calculatedCutoff * (buf0 - buf1);
    buf2 += calculatedCutoff * (buf1 - buf2);
    buf3 += calculatedCutoff * (buf2 - buf3);
    switch (mode) {
        case FILTER_MODE_LOWPASS:
            return buf3;
        case FILTER_MODE_HIGHPASS:
            return inputValue - buf3;
        case FILTER_MODE_BANDPASS:
            return buf0 - buf3;
        default:
            return 0.0;
    }
}

double Filter::getCalculatedCutoff() const {
    return fmax(fmin(cutoff + cutoffMod, 0.99), 0.01);
}

void Filter::calculateFeedbackAmount() {
    feedbackAmount = resonance + resonance/(1.0 - getCalculatedCutoff());
}

void Filter::setCutoffMod(double newCutoffMod) {
    cutoffMod = newCutoffMod;
    calculateFeedbackAmount();
}
