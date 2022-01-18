#ifndef PADSYNTHALGORITHM_H
#define PADSYNTHALGORITHM_H

#include <QObject>
#include <QAudioOutput>
#include <QByteArray>
#include <QFile>
#include "fftwrapper/FFTwrapper.h"


class PadSynthAlgorithm : public QObject
{
    Q_OBJECT
public:
    explicit PadSynthAlgorithm(QObject *parent = nullptr);
    ~PadSynthAlgorithm();
    /*  generates the wavetable
    f		- the fundamental frequency (eg. 440 Hz)
    bw		- bandwidth in cents of the fundamental frequency (eg. 25 cents)
    bwscale	- how the bandwidth increase on the higher harmonics (recomanded value: 1.0)
    *smp	- a pointer to allocated memory that can hold N samples
    N                   - is the samplesize (eg: 262144)
    samplerate          - samplerate (eg. 44100)
    number_harmonics    - the number of harmonics that are computed */

    void padsynth_basic_algorithm(int N,
                                    int samplerate,
                                    float f,
                                    float bw,
                                    int number_harmonics,
                                    float* A, /*input data*/
                                    float* smp, /*output data*/
                                    QVector<float> &spectrum_amplitude
                                  );


    int getN() const;

    QVector<float> getSpectrum_amplitude() const;

    QVector<float> getHarmonics_amplitude() const;

    int getNumber_harmonics() const;

    int getSamplerate() const;

    QVector<float> getHarmonics_amplitude_resampled() const;

    QVector<float> getSpectrum_amplitude_resampled() const;

    QVector<float> getSound_samples() const;

protected:
    int N = 44100;//262144;			//Size of the sample

    /* IFFT() - inverse fast fourier transform
   YOU MUST IMPLEMENT THIS METHOD!
   *freq_real and *freq_imaginary represents the real and the imaginary part of the spectrum,
   The result should be in *smp array.
   The size of the *smp array is N and the size of the freq_real and freq_imaginary is N/2 */
    void IFFT(int N, float* freq_amp, float* freq_phase, float* smp);

    /* profile():
    This is the profile of one harmonic
    In this case is a Gaussian distribution (e^(-x^2))
    The amplitude is divided by the bandwidth to ensure that the harmonic
    keeps the same amplitude regardless of the bandwidth */
    float profile(float fi, float bwi);

    /* RND() - a random number generator that
    returns values between 0 and 1*/
    float RND();

    void normalize(int N, float* smp);

    void save_to_file();
    void save_to_file(const QVector<float>& samples);
    void apply_fade_in_out(QVector<float>& samples);

private:
    float *A_;		//Amplitude of the harmonics
    float *freq_amp_;	//Amplitude spectrum
    QVector<float>spectrum_amplitude_;
    QVector<float>harmonics_amplitude_;
    QVector<float>harmonics_amplitude_resampled_;
    QVector<float>spectrum_amplitude_resampled_;
    int samplerate_ = 44100;
    int number_harmonics_ = 32;
    QVector<int> isample_;
    QVector<float> sound_samples_;
    QVector<float> sound_samples_alt_;
//    REALTYPE* sample_;

//    QFile sourceFile_;
    QAudioOutput* audio_;
//    QByteArray byte_array_data_;

private slots:
    void handleStateChanged(QAudio::State newState);

public slots:
//    void doWork(const QString &parameter) {
//        QString result;
        /* ... here is the expensive or blocking operation ... */
//        emit resultReady(result);
//    }
    void process();
signals:
//    void resultReady(const QString &result);
};

#endif // PADSYNTHALGORITHM_H
