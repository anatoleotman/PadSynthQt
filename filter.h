#ifndef FILTER_H
#define FILTER_H

#include <QObject>

class Filter : public QObject
{
    Q_OBJECT
public:
    explicit Filter(QObject *parent = nullptr);
    enum FilterMode {
        FILTER_MODE_LOWPASS = 0,
        FILTER_MODE_HIGHPASS,
        FILTER_MODE_BANDPASS,
        kNumFilterModes
    };

    double process(double inputValue);
//    inline void setCutoff(double newCutoff) { cutoff = newCutoff; calculateFeedbackAmount(); };
//    inline void setResonance(double newResonance) { resonance = newResonance; calculateFeedbackAmount(); };
//    inline void setFilterMode(FilterMode newMode) { mode = newMode; }

    void setCutoffMod(double newCutoffMod);
private:
    double cutoff;
    double resonance;
    FilterMode mode;
    double buf0;
    double buf1;
    double buf2;
    double buf3;

    double cutoffMod;

    double feedbackAmount;

    void calculateFeedbackAmount();
    double getCalculatedCutoff() const;
signals:

};

#endif // FILTER_H

