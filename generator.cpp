#include "generator.h"
#include <cstring>
#include <QDebug>
#include <QDataStream>
#include <QBuffer>

#include <QVector>

Generator::Generator(QObject *parent)
            : QObject(parent),
              synth_(44100*6, 44100, 64, 260.137 / 2, 260.137, this)
{
    this->init_wavetables();
//    this->start();
}

Generator::~Generator()
{
    qDebug() << "Generator::~Generator()";
    this->io_->deleteLater();
    this->stop();
    this->audio_->stop();
    this->audio_->deleteLater();
}

void Generator::start()
{
//    this->open(QIODevice::ReadOnly);
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(32);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::Float);
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }
    this->audio_ = new QAudioOutput(info, format, this);
    connect(this->audio_, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
//      Init pull mode
//    QBuffer* input = new QBuffer(&this->byte_array_);
//    input->open(QIODevice::ReadOnly);
//    audio_->start(input);

//      Init push mode
    this->io_ = this->audio_->start();
    if (this->audio_->state() == QAudio::StoppedState) {
        qDebug() << "Generator::Generator QAudio::StoppedState";
    }
    if (!this->io_)
    {
        qDebug() << "Generator::Generator Failed to open output audio device";
        return;
    }
    QTimer *timer_push = new QTimer(this);
    connect(timer_push, &QTimer::timeout, this, &Generator::push);
    timer_push->start(20);

    // test to re-init data
//    QTimer *timer_data = new QTimer(this);
//    connect(timer_data, &QTimer::timeout, this, &Generator::init_wavetables);
//    timer_data->start(3000);

}

void Generator::stop()
{
    this->pos_ = 0;
//    this->close();
}

void Generator::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
        case QAudio::ActiveState:
            qDebug() << "Generator::handleStateChanged QAudio::ActiveState";
            break;

        case QAudio::IdleState:
            // Finished playing (no more data)
            qDebug() << "Generator::handleStateChanged Finished playing (no more data)";
//            audio_->stop();
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

void Generator::push()
{
    if (this->audio_->state() == QAudio::StoppedState)
        return;

    QByteArray buffer(32768, 0);
    int chunks = this->audio_->bytesFree() / this->audio_->periodSize();
//    qDebug() << "Generator::push()" << chunks << "chunks" << "this->audio_->periodSize()" << this->audio_->periodSize();
    while (chunks) {
//        const qint64 len = this->read(buffer.data(), this->audio_->periodSize());
        const qint64 len = this->readData(buffer, this->audio_->periodSize());
//        qDebug() << "Generator::push() readData" << len << "this->audio_->periodSize()" << this->audio_->periodSize();
        if (len) this->io_->write(buffer.data(), len);
        if (len != this->audio_->periodSize()) {
            qDebug() << "Generator::push len = this->read != this->audio_->periodSize() break";
//            this->audio_->stop();
            break;
        }
        --chunks;
    }
}

qint64 Generator::readData(char *data, qint64 len)
{
    qint64 total = 0;
    if (!this->byte_array_.isEmpty()) {
        while (len - total > 0) {
            const qint64 chunk = qMin((this->byte_array_.size() - this->pos_), len - total);
            std::memcpy(data + total, this->byte_array_.constData() + this->pos_, chunk);
            total = total + chunk;
            this->pos_ = (this->pos_ + chunk) % this->byte_array_.size();
        }
    }
    return total;
}

qint64 Generator::readData(QByteArray &byte_array, qint64 maxlen)
{
    qint64 total = 0;
    if (!this->byte_array_.isEmpty()) {
        while (maxlen - total > 0) {
            const qint64 chunk = qMin((this->byte_array_.size() - this->pos_), maxlen - total);
//            std::memcpy(byte_array.data() + total,
//                        this->byte_array_.constData() + this->pos_,
//                        chunk);
            std::copy(this->byte_array_.constBegin() + this->pos_,
                      this->byte_array_.constBegin() + this->pos_ + chunk,
                      byte_array.begin() + total);
            total = total + chunk;
            this->pos_ = (this->pos_ + chunk) % this->byte_array_.size();
//            this->pos_ = this->pos_ + chunk;
//            if (this->pos_ >= this->byte_array_.size()-1) {
//                total = 0;
//                break;
//            }
        }
    }
    return total;
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

void Generator::init_wavetables()
{
    //    QByteArray* byte_array_data_ = new QByteArray();
    if (this->byte_array_.size()) {
        this->byte_array_.clear();
        qDebug() << "Generator::init_wavetables this->byte_array_.clear()";
    }
    QDataStream stream(&this->byte_array_, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
//    stream << this->synth_.getWavetable();
    for (const float& f: this->synth_.getWavetable()) {
        stream << f;
    }
}

//qint64 Generator::bytesAvailable() const
//{
//    return byte_array_.size() + QIODevice::bytesAvailable();
//}
