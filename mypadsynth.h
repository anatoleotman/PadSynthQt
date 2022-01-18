#ifndef MYPADSYNTH_H
#define MYPADSYNTH_H

#include <QObject>
#include "fftwrapper/FFTwrapper.h"

#include "envelopegenerator.h"
#include "filter.h"

class MyPadSynth : public QObject
{
    Q_OBJECT
public:
    explicit MyPadSynth(int N,
                        int samplerate,
                        int number_harmonics,
                        float fundamental_frequency,
                        float base_frequency,
                        QObject *parent = nullptr);
    /*  PADsynth:
        N                - is the samplesize (eg: 262144)
        samplerate 	 - samplerate (eg. 44100)
        number_harmonics - the number of harmonics that are computed */
//	PADsynth(int N_, int samplerate_, int number_harmonics_);

    ~MyPadSynth();

    /* set the amplitude of the n'th harmonic */
    void setharmonic(int n, float value);

    /* get the amplitude of the n'th harmonic */
    float getharmonic(int n);

    /*  synth() generates the wavetable
        f		- the fundamental frequency (eg. 440 Hz)
        bw		- bandwidth in cents of the fundamental frequency (eg. 25 cents)
        bwscale	- how the bandwidth increase on the higher harmonics (recomanded value: 1.0)
        *smp	- a pointer to allocated memory that can hold N samples */
    void synth(float f1, float bandwidth, float bandwidth_scale, QVector<float> &samples);

    QVector<float> getWavetable() const;

protected:
    int wavetable_size_;			//Size of the sample

    /* IFFT() - inverse fast fourier transform
       YOU MUST IMPLEMENT THIS METHOD!
       *freq_real and *freq_imaginary represents the real and the imaginary part of the spectrum,
       The result should be in *smp array.
       The size of the *samples array is N and the size of the freq_real and freq_imaginary is N/2 */
    virtual void IFFT(QVector<float> &freq_real, QVector<float> &freq_imaginary, QVector<float> &samples);

    /* relF():
        This method returns the N'th overtone's position relative
        to the fundamental frequency.
        By default it returns N.
        You may override it to make metallic sounds or other
        instruments where the overtones are not harmonic.  */
    virtual float relF(int N);

    /* profile():
        This is the profile of one harmonic
        In this case is a Gaussian distribution (e^(-x^2))
            The amplitude is divided by the bandwidth to ensure that the harmonic
        keeps the same amplitude regardless of the bandwidth */
    virtual float profile(float fi, float bwi);

    /* RND() - a random number generator that
        returns values between 0 and 1
    */
    virtual float RND();

    void normalize(QVector<float> &samples);

    void build_harmonics();
    void resample_harmonics(float resampling_factor);

private:
    QVector<float> A_;		//Amplitude of the harmonics
    QVector<float> freq_amp_;	//Amplitude spectrum
    int samplerate_;
    int number_harmonics_;
    float fundamental_frequency_;
    float base_frequency_;

    QVector<float> wavetable_;

    EnvelopeGenerator* envelope_generator_;
    Filter* filter_;
    EnvelopeGenerator* filter_envelope_generator_;
    double filterEnvelopeAmount_;
signals:

};

#endif // MYPADSYNTH_H
