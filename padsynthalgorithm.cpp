#include "padsynthalgorithm.h"

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
//#define PI 3.14159265358979;

#include <QAudioFormat>
#include <QDebug>
#include <QDataStream>
#include <QBuffer>
#include <QVector>

#include <armadillo>
//https://michaelkrzyzaniak.com/AudioSynthesis/2_Audio_Synthesis/3_Envelopes/4_AR/
//https://www.acs.psu.edu/drussell/Piano/Dynamics.html

PadSynthAlgorithm::PadSynthAlgorithm(QObject *parent) : QObject(parent)
{
    srand(time(0));
//    sample_ = new REALTYPE[N];
    QVector<float> sample(N);
    QVector<float> sample_alt(N);
    float A[number_harmonics_];
    A[0] = 0.0;//A[0] is not used
    this->harmonics_amplitude_ << A[0];
//    float f1 = 130.81; //
    float base_frequency = 130.81;
    float f1 = base_frequency+4;
//    float f1 = 260.737;
//    float f1 = 440;
//    float f1 = 880;
    float resampling_factor = base_frequency / f1;

//    for (int i = 1; i < this->number_harmonics_; i++) {
//        A[i] = 1.0 / i;
//        if ((i % 2) == 0) A[i] *= 2.0;
//        this->harmonics_amplitude_ << A[i];
//    }

    for (int i = 1; i < this->number_harmonics_; i++) {
        A[i] = 1.0 / i;
        float formants = exp(-pow((i * f1 - 600.0) / 150.0, 2.0)) +
                            exp(-pow((i * f1 - 900.0) / 250.0, 2.0)) +
                            exp(-pow((i * f1 - 2200.0) / 200.0, 2.0)) +
                            exp(-pow((i * f1 - 2600.0) / 250.0, 2.0)) +
                            exp(-pow((i * f1) / 3000.0, 2.0)) * 0.1;
        A[i] *= formants;
        this->harmonics_amplitude_ << A[i];
    }
    int resampled_harmonics_amplitude_number = (int)round(this->harmonics_amplitude_.size() * resampling_factor);
    if (resampled_harmonics_amplitude_number != this->harmonics_amplitude_.size()) {
        // resample this->harmonics_amplitude_
        qDebug() << "PadSynthAlgorithm::PadSynthAlgorithm resample harmonics_amplitude size" << resampled_harmonics_amplitude_number;
        std::vector<float> harmonics_amplitude = std::vector<float>(this->harmonics_amplitude_.begin(), this->harmonics_amplitude_.end());
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
        padsynth_basic_algorithm(this->N, this->samplerate_, f1, 40.0, resampled_harmonics_amplitude.size(), resampled_harmonics_amplitude.data(), sample_alt.data(), this->spectrum_amplitude_resampled_);
//        this->number_harmonics_ = resampled_harmonics_amplitude.size();
        this->harmonics_amplitude_resampled_.reserve(resampled_harmonics_amplitude.size());
        this->harmonics_amplitude_resampled_ << resampled_harmonics_amplitude;
    }
    //    padsynth_basic_algorithm(N, 44100, f1, 1.0, number_harmonics_, A, sample.data());
    //    padsynth_basic_algorithm(this->N, this->samplerate_, f1, 40.0, this->number_harmonics_, A, sample.data());
    padsynth_basic_algorithm(this->N, this->samplerate_, f1, 40.0, this->number_harmonics_, this->harmonics_amplitude_.data(), sample.data(), this->spectrum_amplitude_);
    /* Output the data to the 16 bit, mono raw file */
//    int* isample = new int[N];
//    QVector<float> sound_samples;
    this->sound_samples_.reserve(4*sample.size());
    this->sound_samples_alt_.reserve(4*sample_alt.size());
    this->sound_samples_ << sample << sample << sample << sample;
    this->sound_samples_alt_ << sample_alt << sample_alt << sample_alt << sample_alt;
    this->apply_fade_in_out(this->sound_samples_);
    this->apply_fade_in_out(this->sound_samples_alt_);
//    QVector<int> isample(sound_samples.size());

//    this->isample_.resize(this->sound_samples_.size());
//    for (int i = 0; i < this->sound_samples_.size(); i++) {
//        //isample[i] = ( int)(sample[i] * 32767);
//        isample_[i] = (int)(this->sound_samples_.at(i) * 2147483647.0); // 32 bits 4 bytes
//    }

//    FILE* f = fopen("sample.raw", "wb");
//    fwrite(isample_.constData(), sizeof(int), N, f);
//    fclose(f);

//    this->save_to_file();
//    this->save_to_file(sound_samples_);

//    QVector<int> sound_samples;
//    sound_samples.reserve(4*N);
//    sound_samples << isample << isample << isample;// << isample;
//    this->apply_fade_in_out(sound_samples);
    //    build_sin_wave(N, 44100);

//    sourceFile_.setFileName("sample.raw");
//    sourceFile_.open(QIODevice::ReadOnly);

}

PadSynthAlgorithm::~PadSynthAlgorithm()
{
    qDebug() << "PadSynthAlgorithm::~PadSynthAlgorithm()";
}

void PadSynthAlgorithm::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
        case QAudio::IdleState:
            // Finished playing (no more data)
            qDebug() << "PadSynthAlgorithm::handleStateChanged Finished playing (no more data)";
            audio_->stop();
////            sourceFile.close();
//            delete audio_;
            break;

        case QAudio::StoppedState:
            // Stopped for other reasons
            if (audio_->error() != QAudio::NoError) {
                // Error handling
                qDebug() << "QAudio error";
            }
            break;

        default:
            // ... other cases as appropriate
            break;
    }
}

void PadSynthAlgorithm::process()
{
    QByteArray* byte_array_data_ = new QByteArray();
    QDataStream stream(byte_array_data_, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
//    stream << this->isample_;
    stream << this->sound_samples_;
//    stream << isample;
//    stream << isample;
//    stream << sample_;
//    for (const int& i: qAsConst(isample)) {
//        stream << i;
//    }
//    for (const int& i: qAsConst(isample)) {
//        stream << i;
//    }
//    for (const int& i: qAsConst(isample)) {
//        stream << i;
//    }
//    for (const int& i: qAsConst(isample)) {
//        stream << i;
//    }
//    stream << sound_samples;
    QBuffer* input = new QBuffer(byte_array_data_);
    input->open(QIODevice::ReadOnly);

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(32);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
//    format.setSampleType(QAudioFormat::SignedInt);
    format.setSampleType(QAudioFormat::Float);
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    audio_ = new QAudioOutput(info, format, this);
    connect(audio_, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
//    audio_->start(&sourceFile_);
    audio_->start(input);

//    //push mode
//    QIODevice *output_device = this->audio_->start();
//    if (this->audio_->state() == QAudio::StoppedState) {
//        qDebug() << "PadSynthAlgorithm::process QAudio::StoppedState";
////        return;
//    }
//    if (!output_device)
//    {
//        qDebug() << "PadSynthAlgorithm::process Failed to open output audio device";
//        return;
//    }
//    else {
//        QByteArray data;
//        QDataStream datastream(&data, QIODevice::ReadWrite);
//        datastream.setByteOrder(QDataStream::LittleEndian);
//        datastream << isample_;
//        qint64 bytes_written = output_device->write(data);
//        qDebug() << "PadSynthAlgorithm::process bytes_written" << bytes_written;
//    }
}

/* Random number generator */
float PadSynthAlgorithm::RND() {
    return (rand() / (RAND_MAX + 1.0));
}

/* This is the profile of one harmonic
   In this case is a Gaussian distribution (e^(-x^2))
   The amplitude is divided by the bandwidth to ensure that the harmonic
   keeps the same amplitude regardless of the bandwidth */
float PadSynthAlgorithm::profile(float fi, float bwi) {
    float x = fi / bwi;
    x *= x;
    if (x > 14.71280603) return 0.0;//this avoids computing the e^(-x^2) where it's results are very close to zero
    return exp(-x) / bwi;
}

/*
    Inverse Fast Fourier Transform
    You may replace it with any IFFT routine
*/
void PadSynthAlgorithm::IFFT(int N, float* freq_amp, float* freq_phase, float* smp) {
    FFTwrapper fft(N);
    FFTFREQS fftfreqs;
    newFFTFREQS(&fftfreqs, N / 2);

    for (int i = 0; i < N / 2; i++) {
        fftfreqs.c[i] = freq_amp[i] * cos(freq_phase[i]);
        fftfreqs.s[i] = freq_amp[i] * sin(freq_phase[i]);
    }

    fft.freqs2smps(fftfreqs, smp);
    deleteFFTFREQS(&fftfreqs);
}

/*
    Simple normalization function. It normalizes the sound to 1/sqrt(2)
*/
void PadSynthAlgorithm::normalize(int N, float* smp) {
    int i;
    float max = 0.0;
    for (i = 0; i < N; i++) if (fabs(smp[i]) > max) max = fabs(smp[i]);
    if (max < 1e-5) max = 1e-5;
    for (i = 0; i < N; i++) smp[i] /= max * 1.4142;
}

void PadSynthAlgorithm::save_to_file()
{
    FILE* f = fopen("sample.raw", "wb");
    fwrite(isample_.constData(), sizeof(int), this->N*4, f);
    fclose(f);
}

void PadSynthAlgorithm::save_to_file(const QVector<float> &samples)
{
    FILE* f = fopen("sample_float.raw", "wb");
    fwrite(samples.constData(), sizeof(float), samples.size(), f);
    fclose(f);
}

void PadSynthAlgorithm::apply_fade_in_out(QVector<float> &samples)
{
    float fade = .05 * samplerate_;
    float current_volume;
    for (int i = 0; i < samples.size(); i++) {
        if (i < (int)fade) {
            current_volume = i / fade;
        }
        else if (i >= (samples.size() - fade)) {
            current_volume = 1 - (i - (samples.size()-1-fade)) / fade;
//            if (i == samples.size()-1) {
////                qDebug() << "PadSynthAlgorithm::apply_fade_in_out i == samples.size()-1 : current_volume" << current_volume;
//            }
        }
        else {
            current_volume = 1;
        }
//        samples[i] *= current_volume;
        samples[i] *= pow(current_volume, 2);
    }
}

QVector<float> PadSynthAlgorithm::getSound_samples() const
{
    return sound_samples_alt_;
}

QVector<float> PadSynthAlgorithm::getSpectrum_amplitude_resampled() const
{
    return spectrum_amplitude_resampled_;
}

QVector<float> PadSynthAlgorithm::getHarmonics_amplitude_resampled() const
{
    return harmonics_amplitude_resampled_;
}

int PadSynthAlgorithm::getSamplerate() const
{
    return samplerate_;
}

int PadSynthAlgorithm::getNumber_harmonics() const
{
    return number_harmonics_;
}

QVector<float> PadSynthAlgorithm::getHarmonics_amplitude() const
{
    return harmonics_amplitude_;
}

QVector<float> PadSynthAlgorithm::getSpectrum_amplitude() const
{
    return spectrum_amplitude_;
}

int PadSynthAlgorithm::getN() const
{
    return N;
}

/*
    This is the implementation of PADsynth algorithm.
*/
void PadSynthAlgorithm::padsynth_basic_algorithm(int N, int samplerate, float f, float bw, int number_harmonics, float* A, /*input data*/
        float* smp /*output data*/, QVector<float> &spectrum_amplitude)
{
    int i, nh;
    float* freq_amp_ = new float[N / 2]; //should call it once
    spectrum_amplitude.resize(N / 2);
    float* freq_phase = new float[N / 2];

    for (i = 0; i < N / 2; i++) {
        freq_amp_[i] = 0.0;//default, all the frequency amplitudes are zero
//        spectrum_amplitude[i] = 0.0;
    }
    std::fill(spectrum_amplitude.begin(), spectrum_amplitude.end(), 0.);

    for (nh = 1; nh < number_harmonics; nh++) { //for each harmonic
        float bw_Hz; //bandwidth of the current harmonic measured in Hz
        float bwi;
        float fi;
        bw_Hz = (pow(2.0, bw / 1200.0) - 1.0) * f * nh;

        bwi = bw_Hz / (2.0 * samplerate);
        fi = f * nh / samplerate;
//        qDebug() << "f:" << f << "nh:" << nh << "fi:" << fi;
        for (i = 0; i < N / 2; i++) {
            float hprofile;
            hprofile = this->profile((i / (float)N) - fi, bwi);
            freq_amp_[i] += hprofile * A[nh];
            spectrum_amplitude[i] += hprofile * A[nh];
        }
    }

    //Add random phases
    for (i = 0; i < N / 2; i++) {
        freq_phase[i] = RND() * 2.0 * M_PI;
    }

//    this->IFFT(N, freq_amp_, freq_phase, smp);
    this->IFFT(N, spectrum_amplitude.data(), freq_phase, smp);
    this->normalize(N, smp);

    delete[] freq_amp_;
    delete[] freq_phase;
}

//void build_sin_wave(int N, int samplerate) {
//    double freq = 440;
//    int* isample = new int[N];
//    for (int i = 0; i < N; i++) {
//        isample[i] = (int)floor(sin(2 * M_PI * freq * (double)i / samplerate) * 32767);
//    }
//    FILE* f = fopen("sine.raw", "wb"); fwrite(isample, sizeof(int), N, f); fclose(f);
//}
