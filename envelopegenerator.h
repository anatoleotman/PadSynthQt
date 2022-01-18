#ifndef ENVELOPEGENERATOR_H
#define ENVELOPEGENERATOR_H

#include <QObject>

#include <cmath>

class EnvelopeGenerator : public QObject
{
    Q_OBJECT
public:
    explicit EnvelopeGenerator(QObject *parent = nullptr);
    enum EnvelopeStage {
        ENVELOPE_STAGE_OFF = 0,
        ENVELOPE_STAGE_ATTACK,
        ENVELOPE_STAGE_DECAY,
        ENVELOPE_STAGE_SUSTAIN,
        ENVELOPE_STAGE_RELEASE,
        kNumEnvelopeStages
    };
    void enterStage(EnvelopeStage newStage);
    double nextSample();
    void setSampleRate(double newSampleRate);
    inline EnvelopeStage getCurrentStage() const { return currentStage; };
    const double minimumLevel;
    void setStageValue(EnvelopeStage stage, double value);

private:
    EnvelopeStage currentStage;
    double currentLevel;
    double multiplier;
    double sampleRate;
    double stageValue[kNumEnvelopeStages];
    void calculateMultiplier(double startLevel, double endLevel, unsigned long long lengthInSamples);
    unsigned long long currentSampleIndex;
    unsigned long long nextStageSampleIndex;
signals:

};

#endif // ENVELOPEGENERATOR_H
