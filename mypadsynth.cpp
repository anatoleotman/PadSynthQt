/*
    PADsynth implementation as ready-to-use C++ class.
    By: Nasca O. Paul, Tg. Mures, Romania
    This implementation and the algorithm are released under Public Domain
    Feel free to use it into your projects or your products ;-)

    This implementation is tested under GCC/Linux, but it's
    very easy to port to other compiler/OS. */

#include "mypadsynth.h"

#include <QDebug>
//#include <stdio.h>
//#include <stdlib.h>
#include <math.h>
#include <armadillo>

MyPadSynth::MyPadSynth(int N,
                       int samplerate,
                       int number_harmonics,
                       float fundamental_frequency,
                       float base_frequency,
                       QObject *parent)
                            : QObject(parent)
{
    this->wavetable_size_ = N;
    this->samplerate_ = samplerate;
    this->number_harmonics_ = number_harmonics;
    this->fundamental_frequency_ = fundamental_frequency;
    this->base_frequency_ = base_frequency;
//    float base_frequency = 130.81;
//    float f1 = base_frequency+4;
    this->envelope_generator_ = new EnvelopeGenerator(this);
    this->filter_ = new Filter(this);
    this->filter_envelope_generator_ = new EnvelopeGenerator(this);
    this->filter_envelope_generator_->setStageValue(EnvelopeGenerator::ENVELOPE_STAGE_DECAY, 1.);
    this->filterEnvelopeAmount_ = 0;

    this->A_.resize(number_harmonics);
    for (int i = 0; i < this->A_.size(); i++) this->A_[i] = 0.0;
    this->A_[1] = 1.0; //default, the first harmonic has the amplitude 1.0
    this->build_harmonics();
    this->freq_amp_.resize(N / 2);
    this->wavetable_.resize(N);
//    float f1 = 260.737;
//    float f1 = 440;
//    float f1 = 880;
    float resampling_factor = base_frequency / fundamental_frequency;
    if (resampling_factor != 1.) {
        this->resample_harmonics(resampling_factor);
    }
    float bandwidth = 40; // bandwidth unit is cents ?
    float bandwidth_scale = 1.;
    this->synth(fundamental_frequency, bandwidth, bandwidth_scale, this->wavetable_);

//    // test envelope, filter and envelope filter
//    for (float& sample: this->wavetable_) {
//        if (this->envelope_generator_->getCurrentStage() == EnvelopeGenerator::ENVELOPE_STAGE_OFF) {
//            this->envelope_generator_->enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
//            this->filter_envelope_generator_->enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
//        }
//        if (this->envelope_generator_->getCurrentStage() == EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN) {
//            this->envelope_generator_->enterStage(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
//            this->filter_envelope_generator_->enterStage(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
//        }
////        if (this->filter_envelope_generator_->getCurrentStage() == EnvelopeGenerator::ENVELOPE_STAGE_OFF) {
////            this->filter_envelope_generator_->enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
////        }
////        if (this->filter_envelope_generator_->getCurrentStage() == EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN) {
////            this->filter_envelope_generator_->enterStage(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
////        }
////        sample *= this->envelope_generator_->nextSample();
//        this->filter_->setCutoffMod(this->filter_envelope_generator_->nextSample() * this->filterEnvelopeAmount_);
//        sample = this->filter_->process(sample * this->envelope_generator_->nextSample());
//    }
}

MyPadSynth::~MyPadSynth() {
    qDebug() << "MyPadSynth::~MyPadSynth()";
};

float MyPadSynth::relF(int N) {
    return N;
};

void MyPadSynth::setharmonic(int n, float value) {
    if ((n < 1) || (n >= this->A_.size())) return;
    this->A_[n] = value;
};

float MyPadSynth::getharmonic(int n) {
    if ((n < 1) || (n >= this->A_.size())) return 0.0;
    return this->A_[n];
};

float MyPadSynth::profile(float fi, float bwi) {
    float x = fi / bwi;
    x *= x;
    if (x > 14.71280603) return 0.0;//this avoids computing the e^(-x^2) where it's results are very close to zero
    return exp(-x) / bwi;
};

void MyPadSynth::synth(float f1, float bandwidth, float bandwidth_scale, QVector<float> &samples) {
    for (int i = 0; i < this->wavetable_size_ / 2; i++) this->freq_amp_[i] = 0.0;//default, all the frequency amplitudes are zero

    for (int nh = 1; nh < this->A_.size(); nh++) {//for each harmonic
        float bw_Hz;//bandwidth of the current harmonic measured in Hz
        float bwi;
        float fi;
        float rF = f1 * this->relF(nh);

        bw_Hz = (pow(2.0, bandwidth / 1200.0) - 1.0) * f1 * pow(this->relF(nh), bandwidth_scale);

        bwi = bw_Hz / (2.0 * this->samplerate_);
        fi = rF / this->samplerate_;
        for (int i = 0; i < this->wavetable_size_ / 2; i++) {//here you can optimize, by avoiding to compute the profile for the full frequency (usually it's zero or very close to zero)
            float hprofile;
            hprofile = this->profile((i / (float)this->wavetable_size_) - fi, bwi);
            this->freq_amp_[i] += hprofile * this->A_[nh];
        };
    };

    QVector<float> freq_real(this->wavetable_size_ / 2);
    QVector<float> freq_imaginary(this->wavetable_size_ / 2);

    //Convert the freq_amp array to complex array (real/imaginary) by making the phases random
    for (int i = 0; i < this->wavetable_size_ / 2; i++) {
        float phase = RND() * 2.0 * 3.14159265358979;
        freq_real[i] = this->freq_amp_[i] * cos(phase);
        freq_imaginary[i] = this->freq_amp_[i] * sin(phase);
    };
    this->IFFT(freq_real, freq_imaginary, samples);

    //normalize the output
    this->normalize(samples);
};

/*
    Simple normalization function. It normalizes the sound to 1/sqrt(2)
*/
void MyPadSynth::normalize(QVector<float> &samples) {
    float max = 0.0;
    for (int i = 0; i < samples.size(); i++) {
        if (fabs(samples[i]) > max) {
            max = fabs(samples[i]);
        }
    }
    if (max < 1e-5) {
        max = 1e-5;
    }
    for (int i = 0; i < samples.size(); i++) {
        samples[i] /= max * 1.4142;
    }
}

void MyPadSynth::build_harmonics()
{
    for (int i = 1; i < this->number_harmonics_; i++) {
        this->A_[i] = 1.0 / i;
        float formants = exp(-pow((i * this->fundamental_frequency_ - 600.0) / 150.0, 2.0)) +
                            exp(-pow((i * this->fundamental_frequency_ - 900.0) / 250.0, 2.0)) +
                            exp(-pow((i * this->fundamental_frequency_ - 2200.0) / 200.0, 2.0)) +
                            exp(-pow((i * this->fundamental_frequency_ - 2600.0) / 250.0, 2.0)) +
                            exp(-pow((i * this->fundamental_frequency_) / 3000.0, 2.0)) * 0.1;
        this->A_[i] *= formants;
    }
}

void MyPadSynth::resample_harmonics(float resampling_factor)
{
    int resampled_harmonics_amplitude_number = (int)round(this->A_.size() * resampling_factor);
    if (resampled_harmonics_amplitude_number != this->A_.size()) {
        // resample this->harmonics_amplitude_
        qDebug() << "PadSynthAlgorithm::PadSynthAlgorithm resample harmonics_amplitude size" << resampled_harmonics_amplitude_number;
        std::vector<float> harmonics_amplitude = std::vector<float>(this->A_.begin(), this->A_.end());
        //    qDebug() << "PadSynthAlgorithm::PadSynthAlgorithm harmonics_amplitude" << harmonics_amplitude.size() << harmonics_amplitude;
        arma::frowvec harmonics_amplitude_arma = arma::frowvec(harmonics_amplitude);
        //    harmonics_amplitude_arma.print("harmonics_amplitude_arma");
        arma::frowvec harmonics_amplitude_locations = arma::linspace<arma::frowvec>(1, harmonics_amplitude_arma.size(), harmonics_amplitude_arma.size());
//        harmonics_amplitude_locations.print("harmonics_amplitude_locations");
        arma::frowvec resampled_harmonics_amplitude_locations = arma::linspace<arma::frowvec>(1, harmonics_amplitude_arma.size(), resampled_harmonics_amplitude_number);
        arma::frowvec resampled_harmonics_amplitude_arma;
        arma::interp1(harmonics_amplitude_locations, harmonics_amplitude_arma, resampled_harmonics_amplitude_locations, resampled_harmonics_amplitude_arma, "*linear");
        std::vector<float> resampled_harmonics_amplitude_stdvec(arma::conv_to<std::vector<float>>::from(resampled_harmonics_amplitude_arma));
        QVector<float>resampled_harmonics_amplitude(resampled_harmonics_amplitude_stdvec.begin(), resampled_harmonics_amplitude_stdvec.end());
        this->A_.clear();
        this->A_.reserve(resampled_harmonics_amplitude_number);
        this->A_ << resampled_harmonics_amplitude;
        this->number_harmonics_ = resampled_harmonics_amplitude_number;
        //        arma::frowvec y({1,2,1,3,0,0,1,0});
//        arma::frowvec x = arma::linspace<arma::frowvec>(1, 8, 8);
//        x.print("x");
//        y.print("y");
//        arma::frowvec x_down = arma::linspace<arma::frowvec>(1.5, 7.5, 4);
//        //    arma::frowvec x_down = arma::linspace<arma::frowvec>(0, 7, 7);
//        arma::frowvec x_up = arma::linspace<arma::frowvec>(1, 8, 16);
//        //    arma::frowvec x_up = arma::linspace<arma::frowvec>(0, 7, 9);
//        arma::frowvec y_down;
//        arma::interp1(x, y, x_down, y_down, "*linear");
//        //    arma::interp1(x, dummy, x_down, y_down, "*nearest");
//        x_down.print("x_down");
//        y_down.print("y_down");
//        arma::frowvec y_up;
//        arma::interp1(x, y, x_up, y_up, "*linear");
//        //    arma::interp1(x, dummy, x_up, y_up, "*nearest");
//        x_up.print("x_up");
//        y_up.print("y_up");

//        std::vector<float> z(arma::conv_to<std::vector<float>>::from(y_up));
//        QVector<float>zz(z.begin(), z.end());
//        qDebug() << "PadSynthAlgorithm::PadSynthAlgorithm z" << zz;
//        padsynth_basic_algorithm(this->N, this->samplerate_, f1, 40.0, resampled_harmonics_amplitude.size(), resampled_harmonics_amplitude.data(), sample_alt.data(), this->spectrum_amplitude_resampled_);
//        this->number_harmonics_ = resampled_harmonics_amplitude.size();
//        this->harmonics_amplitude_resampled_.reserve(resampled_harmonics_amplitude.size());
//        this->harmonics_amplitude_resampled_ << resampled_harmonics_amplitude;
    }
}

QVector<float> MyPadSynth::getWavetable() const
{
    return wavetable_;
}

float MyPadSynth::RND() {
    return (rand() / (RAND_MAX + 1.0));
};

/*
    Inverse Fast Fourier Transform
    You may replace it with any IFFT routine
*/
void MyPadSynth::IFFT(QVector<float> &freq_amp, QVector<float> &freq_phase, QVector<float> &samples) {
    FFTwrapper fft(this->wavetable_size_);
    FFTFREQS fftfreqs;
    newFFTFREQS(&fftfreqs, this->wavetable_size_ / 2);

    for (int i = 0; i < this->wavetable_size_ / 2; i++) {
        fftfreqs.c[i] = freq_amp[i] * cos(freq_phase[i]);
        fftfreqs.s[i] = freq_amp[i] * sin(freq_phase[i]);
    }

    fft.freqs2smps(fftfreqs, samples.data());
    deleteFFTFREQS(&fftfreqs);
}
