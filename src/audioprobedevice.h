#ifndef AUDIOPROBEDEVICE_H
#define AUDIOPROBEDEVICE_H

#include <QObject>
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QAudioInput>

class AudioProbeDevice : public QIODevice
{
    Q_OBJECT
public:
    explicit AudioProbeDevice(QObject *parent = nullptr) : QIODevice(parent){}
    QAudioFormat format;

    qint64 readData(char *data, qint64 maxSize) override
    {
        Q_UNUSED(data);
        Q_UNUSED(maxSize);
        return 0;
    }
    qint64 writeData(const char *data, qint64 maxSize) override
    {
        format.setSampleRate(16000);
        format.setChannelCount(1);
        format.setSampleSize(8);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::UnSignedInt);
        QAudioBuffer buffer({data, static_cast<int>(maxSize)}, format);
        emit audioAvailable(buffer);
        return maxSize;
    }
    Q_SIGNAL void audioAvailable(QAudioBuffer buffer);
};

#endif // AUDIOPROBEDEVICE_H
