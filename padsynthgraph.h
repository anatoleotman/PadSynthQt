#ifndef PADSYNTHGRAPH_H
#define PADSYNTHGRAPH_H

#include <QQuickItem>
#include "padsynthalgorithm.h"
#include "generator.h"

#include <QThread>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>

class PadSynthGraph : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QtCharts::QLineSeries* series READ getSeries)
public:
    PadSynthGraph();
    ~PadSynthGraph();
    QtCharts::QLineSeries *getSeries() const;

//public slots:
//    void update_series(QtCharts::QLineSeries* series);
    Q_INVOKABLE void spectrum_update_series(QtCharts::QLineSeries* series, int index);
    Q_INVOKABLE float spectrum_max_y_value(int index);
    Q_INVOKABLE int spectrum_max_x_value();

    Q_INVOKABLE void harmonics_amplitude_update_series(QtCharts::QScatterSeries* series, int index);
    Q_INVOKABLE float harmonics_amplitude_max_value(int index);
    Q_INVOKABLE int harmonics_amplitude_length(int index);

    Q_INVOKABLE void sound_samples_update_series(QtCharts::QLineSeries* series, int index=0);
    Q_INVOKABLE int sound_samples_size(int index=0);

//    Q_INVOKABLE void harmonics_profile_update_series(QtCharts::QLineSeries* series);
    Q_INVOKABLE QVector2D harmonics_profile_min_max(int harmonic_number);

protected:
    PadSynthAlgorithm* pad_synth_algorithm_;
    Generator* generator_;
    QThread workerThread_;

private:
    QtCharts::QLineSeries *series;
    int spectrum_size_ = 2000;

public slots:
//    void handleResults(const QString &);
signals:
//    void operate(const QString &);
    void activate_padsynth();
};

#endif // PADSYNTHGRAPH_H
