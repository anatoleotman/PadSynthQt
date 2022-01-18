#ifndef GENERATOR_H
#define GENERATOR_H

#include "mypadsynth.h"

#include <QIODevice>
#include <QAudioOutput>
#include <QByteArray>
#include <QTimer>

class Generator : public QObject
{
    Q_OBJECT
public:
    explicit Generator(QObject *parent = nullptr);
    ~Generator();

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen);// override;
    qint64 readData(QByteArray& byte_array, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);// override;
//    qint64 bytesAvailable() const;// override;

    void init_wavetables();
private slots:
    void handleStateChanged(QAudio::State newState);
    void push();
private:
    QAudioOutput* audio_;
    QIODevice* io_;

    qint64 pos_ = 0;
    QByteArray byte_array_;

    MyPadSynth synth_;
};

#endif // GENERATOR_H
