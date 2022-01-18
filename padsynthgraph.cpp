#include "padsynthgraph.h"
#include <algorithm>
#include <QTimer>

PadSynthGraph::PadSynthGraph()
{
    this->pad_synth_algorithm_ = new PadSynthAlgorithm;
//    this->pad_synth_algorithm_->process();
    this->pad_synth_algorithm_->moveToThread(&this->workerThread_);
    connect(&this->workerThread_, &QThread::finished, this->pad_synth_algorithm_, &QObject::deleteLater);
//    connect(this, &PadSynthGraph::activate_padsynth, this->pad_synth_algorithm_, &PadSynthAlgorithm::process);

//    connect(worker, &Worker::resultReady, this, &Controller::handleResults);
//    QTimer::singleShot(0, this, &PadSynthGraph::activate_padsynth);
//    emit this->activate_padsynth();

    this->generator_ = new Generator;
    this->generator_->moveToThread(&this->workerThread_);
    connect(&this->workerThread_, &QThread::finished, this->generator_, &QObject::deleteLater);
    connect(this, &PadSynthGraph::activate_padsynth, this->generator_, &Generator::start);
    this->workerThread_.start();
    emit this->activate_padsynth();
}

PadSynthGraph::~PadSynthGraph()
{
    this->workerThread_.quit();
    this->workerThread_.wait();
    this->generator_->deleteLater();
}

QtCharts::QLineSeries *PadSynthGraph::getSeries() const
{
    return series;
}

void PadSynthGraph::spectrum_update_series(QtCharts::QLineSeries *series, int index)
{
    QVector<QPointF> points;
    int spectrum_size;
    if (index == 0) {
        points.resize(this->pad_synth_algorithm_->getSpectrum_amplitude().size());
        spectrum_size = this->pad_synth_algorithm_->getSpectrum_amplitude().size();
        for (int i = 0; i < spectrum_size; i++) {
            float x = i * this->pad_synth_algorithm_->getSamplerate() / this->pad_synth_algorithm_->getN();
            points[i] = QPointF(x, this->pad_synth_algorithm_->getSpectrum_amplitude().at(i) + .1); //logvalueaxis cant plot zero values
        }
    }
    else if (index == 1) {
        points.resize(this->pad_synth_algorithm_->getSpectrum_amplitude_resampled().size());
        spectrum_size = this->pad_synth_algorithm_->getSpectrum_amplitude_resampled().size();
        for (int i = 0; i < spectrum_size; i++) {
            float x = i * this->pad_synth_algorithm_->getSamplerate() / this->pad_synth_algorithm_->getN();
            points[i] = QPointF(x, this->pad_synth_algorithm_->getSpectrum_amplitude_resampled().at(i) + .1); //logvalueaxis cant plot zero values
        }
    }
    series->replace(points);
}

float PadSynthGraph::spectrum_max_y_value(int index)
{
    float max = 0;
    if (index == 0) {
        max = *std::max_element(this->pad_synth_algorithm_->getSpectrum_amplitude().constBegin(), this->pad_synth_algorithm_->getSpectrum_amplitude().constEnd());
    }
    else if (index == 1) {
        max = *std::max_element(this->pad_synth_algorithm_->getSpectrum_amplitude_resampled().constBegin(), this->pad_synth_algorithm_->getSpectrum_amplitude_resampled().constEnd());
    }
//    qDebug() << "PadSynthGraph::spectrum_max_value" << max;
//    qDebug() << "index of max" << std::distance(this->pad_synth_algorithm_->getAmplitude_spectrum().constBegin(), this->pad_synth_algorithm_->getAmplitude_spectrum().constEnd());
    return max;
}

int PadSynthGraph::spectrum_max_x_value()
{
    return this->pad_synth_algorithm_->getSamplerate() / 2;
}

void PadSynthGraph::harmonics_amplitude_update_series(QtCharts::QScatterSeries *series, int index)
{
    QVector<QPointF> points;
    if (index == 0) {
        points.resize(this->pad_synth_algorithm_->getNumber_harmonics());
        for (int i = 0; i < this->pad_synth_algorithm_->getNumber_harmonics(); i++) {
            points[i] = QPointF(i, this->pad_synth_algorithm_->getHarmonics_amplitude().at(i));
        }
    }
    else if (index == 1) {
        points.resize(this->pad_synth_algorithm_->getHarmonics_amplitude_resampled().size());
        for (int i = 0; i < this->pad_synth_algorithm_->getHarmonics_amplitude_resampled().size(); i++) {
            points[i] = QPointF(i, this->pad_synth_algorithm_->getHarmonics_amplitude_resampled().at(i));
        }
    }
    series->replace(points);
}

float PadSynthGraph::harmonics_amplitude_max_value(int index)
{
    float max = 0;
    if (index == 0) {
        max = *std::max_element(this->pad_synth_algorithm_->getHarmonics_amplitude().constBegin(), this->pad_synth_algorithm_->getHarmonics_amplitude().constEnd());
    }
    else if (index == 1) {
        max = *std::max_element(this->pad_synth_algorithm_->getHarmonics_amplitude_resampled().constBegin(), this->pad_synth_algorithm_->getHarmonics_amplitude_resampled().constEnd());
    }
    return max;
}

int PadSynthGraph::harmonics_amplitude_length(int index)
{
    int harmonics_amplitude_length = 0;
    if (index == 0) {
        harmonics_amplitude_length = this->pad_synth_algorithm_->getNumber_harmonics();
    }
    else if (index == 1) {
        harmonics_amplitude_length = this->pad_synth_algorithm_->getHarmonics_amplitude_resampled().size();
    }
    return harmonics_amplitude_length;
}

void PadSynthGraph::sound_samples_update_series(QtCharts::QLineSeries *series, int index)
{
    Q_UNUSED(index);
    QVector<QPointF> points;
    points.resize(this->pad_synth_algorithm_->getSound_samples().size());
    int sound_samples_size = this->pad_synth_algorithm_->getSound_samples().size();
    for (int i = 0; i < sound_samples_size; i++) {
//        float x = i * this->pad_synth_algorithm_->getSamplerate() / this->pad_synth_algorithm_->getN();
        points[i] = QPointF(i, this->pad_synth_algorithm_->getSound_samples().at(i));
    }
    series->replace(points);
}

int PadSynthGraph::sound_samples_size(int index)
{
    Q_UNUSED(index);
    return this->pad_synth_algorithm_->getSound_samples().size();
}



QVector2D PadSynthGraph::harmonics_profile_min_max(int harmonic_number)
{
    Q_UNUSED(harmonic_number);
    return QVector2D(220, 300);
}

//void PadSynthGraph::harmonics_profile_update_series(QtCharts::QLineSeries *series)
//{

//}

