#include "audiorecorder.h"

AudioRecorder::AudioRecorder(QObject *parent) : QObject(parent)
{
    audioRecorder = new QAudioRecorder(this);
    m_probe = new QAudioProbe(this);
    connect(m_probe, &QAudioProbe::audioBufferProbed, this, &AudioRecorder::processBuffer);
    m_probe->setSource(audioRecorder);

    statusSpeech = false;
    newStatusSpeech = true;
    timerOfFinishedSpeech.setInterval(2000);
    connect(&timerOfFinishedSpeech, &QTimer::timeout, this, &AudioRecorder::timeoutOfSpeech);

    voiceCommandWaitTimer.setInterval(intervalRecording);
    connect(&voiceCommandWaitTimer, &QTimer::timeout, this, &AudioRecorder::restartWaitTimer);

    modeAudioInput = "voiceStandby";
}

AudioRecorder::~AudioRecorder()
{
    delete audioRecorder;
    delete m_probe;
}

void AudioRecorder::toggleRecord(bool commandRecord)
{
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/x-flac");
    audioSettings.setSampleRate(16000);
    audioSettings.setQuality(QMultimedia::VeryHighQuality);
    QString container = "Default";  //.wav
    audioRecorder->setEncodingSettings(audioSettings);

//    QAudioEncoderSettings settings;
//    settings.setCodec("audio/x-vorbis");
//    settings.setSampleRate(16000);
//    settings.setBitRate(64000);
//    settings.setChannelCount(1);
//    settings.setQuality(QMultimedia::VeryHighQuality);
//    settings.setEncodingMode(QMultimedia::ConstantQualityEncoding);
//    QString container = "audio/ogg";
//    audioRecorder->setEncodingSettings(settings, QVideoEncoderSettings(), container);

    QString fileName = QUuid::createUuid().toString();
    filePath=QDir::currentPath();
    filePath += "/" + fileName;
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(filePath));

    modeAudioInput = "voiceStandby";
    if (commandRecord) {
        audioRecorder->record();
        temporaryFile.setFileName(fileName);
        voiceCommandWaitTimer.start();
    }
    if (!commandRecord) {
        audioRecorder->stop();
        voiceCommandWaitTimer.stop();
        timerOfFinishedSpeech.stop();
        temporaryFile.remove();
    }
}

static qreal getPeakValue(const QAudioFormat &format);
static QVector<qreal> getBufferLevels(const QAudioBuffer &buffer);

template <class T>
static QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels);

// This function returns the maximum possible sample value for a given audio format
qreal getPeakValue(const QAudioFormat& format)
{
    // Note: Only the most common sample formats are supported
    if (!format.isValid())
        return qreal(0);

    if (format.codec() != "audio/pcm")
        return qreal(0);

    switch (format.sampleType()) {
    case QAudioFormat::Unknown:
        break;
    case QAudioFormat::Float:
        if (format.sampleSize() != 32) // other sample formats are not supported
            return qreal(0);
        return qreal(1.00003);
    case QAudioFormat::SignedInt:
        if (format.sampleSize() == 32)
            return qreal(INT_MAX);
        if (format.sampleSize() == 16)
            return qreal(SHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(CHAR_MAX);
        break;
    case QAudioFormat::UnSignedInt:
        if (format.sampleSize() == 32)
            return qreal(UINT_MAX);
        if (format.sampleSize() == 16)
            return qreal(USHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(UCHAR_MAX);
        break;
    }

    return qreal(0);
}

// returns the audio level for each channel
QVector<qreal> getBufferLevels(const QAudioBuffer& buffer)
{
    QVector<qreal> values;

    if (!buffer.format().isValid() || buffer.format().byteOrder() != QAudioFormat::LittleEndian)
        return values;

    if (buffer.format().codec() != "audio/pcm")
        return values;

    int channelCount = buffer.format().channelCount();
    values.fill(0, channelCount);
    qreal peak_value = getPeakValue(buffer.format());
    if (qFuzzyCompare(peak_value, qreal(0)))
        return values;

    switch (buffer.format().sampleType()) {
    case QAudioFormat::Unknown:
    case QAudioFormat::UnSignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<quint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<quint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<quint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] = qAbs(values.at(i) - peak_value / 2) / (peak_value / 2);
        break;
    case QAudioFormat::Float:
        if (buffer.format().sampleSize() == 32) {
            values = getBufferLevels(buffer.constData<float>(), buffer.frameCount(), channelCount);
            for (int i = 0; i < values.size(); ++i)
                values[i] /= peak_value;
        }
        break;
    case QAudioFormat::SignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<qint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<qint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<qint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] /= peak_value;
        break;
    }

    return values;
}
template <class T>

QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels)
{
    QVector<qreal> max_values;
    max_values.fill(0, channels);
    for (int i = 0; i < frames; ++i) {
        for (int j = 0; j < channels; ++j) {
            qreal value = qAbs(qreal(buffer[i * channels + j]));
            if (value > max_values.at(j))
                max_values.replace(j, value);
        }
    }
    return max_values;
}

void AudioRecorder::processBuffer(const QAudioBuffer& buffer)
{
    QVector<qreal> levels = getBufferLevels(buffer);
    for (int i = 0; i < levels.count(); ++i) {
        if (levels.at(i)*10.0 < thresholdOfAudioinputActivation) {
            statusSpeech = false;
        }
        else {
            statusSpeech = true;
        }
    }
    if (!statusSpeech && newStatusSpeech) {
        newStatusSpeech = statusSpeech;
        timerOfFinishedSpeech.start();
    }
    if (statusSpeech && !newStatusSpeech) {
        newStatusSpeech = statusSpeech;
        timerOfFinishedSpeech.stop();
        voiceCommandWaitTimer.stop();
    }
}

void AudioRecorder::timeoutOfSpeech()
{
    timerOfFinishedSpeech.stop();
    voiceCommandWaitTimer.stop();
    audioRecorder->stop();
    modeAudioInput = "speechRecording";
    sendPathToAudioFile(filePath);
    restartWaitTimer();
}

void AudioRecorder::restartWaitTimer()
{
    audioRecorder->stop();
    if (modeAudioInput == "voiceStandby") {
        temporaryFile.remove();
    }
    toggleRecord(true);
}
