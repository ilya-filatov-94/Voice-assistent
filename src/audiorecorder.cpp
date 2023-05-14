#include "audiorecorder.h"



AudioRecorder::AudioRecorder(QObject *parent) : QObject(parent)
{
    audioRecorder = new QAudioRecorder(this);
    setAudioRecorderSettings();
    audioProbe = new AudioProbeDevice(this);
    setAudioBufferSettings();
    statusSpeech = false;
    newStatusSpeech = false;
    statusTrigger = false;
    thresholdOfAudioinputActivation = 40.0;
    timerOfFinishedSpeech.setInterval(2000);
    connect(&timerOfFinishedSpeech, &QTimer::timeout, this, &AudioRecorder::timeoutOfSpeech);
}

AudioRecorder::~AudioRecorder()
{
    delete audioRecorder;
    delete audioProbe;
    delete audioSource;
}

void AudioRecorder::setAudioBufferSettings()
{
    QAudioDeviceInfo inputDevice = QAudioDeviceInfo::defaultInputDevice();
    audioProbe->setCurrentReadChannel(inputDevice.supportedChannelCounts().at(0));
    connect(audioProbe, &AudioProbeDevice::audioAvailable, this, &AudioRecorder::processBuffer);
    QAudioFormat format;
    format.setSampleRate(16000);
    format.setChannelCount(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    audioSource = new QAudioInput(inputDevice, format, this);
    audioSource->setBufferSize(32);
}

void AudioRecorder::setAudioRecorderSettings()
{
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/x-flac");
    audioSettings.setSampleRate(16000);
    audioSettings.setBitRate(96000);
    audioSettings.setChannelCount(1);
    audioSettings.setQuality(QMultimedia::VeryHighQuality);
    audioSettings.setEncodingMode(QMultimedia::ConstantQualityEncoding);
    audioRecorder->setEncodingSettings(audioSettings);
    fileName = QUuid::createUuid().toString();
    filePath = QDir::currentPath();
    filePath += "/" + fileName;
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(filePath));
    listAudioFiles << filePath;
}

void AudioRecorder::toggleRecord(bool commandRecord)
{
    if (commandRecord) {
        audioProbe->open(QIODevice::WriteOnly);
        audioSource->start(audioProbe);
    }
    if (!commandRecord) {
        audioSource->stop();
        audioProbe->close();

        audioRecorder->stop();
        timerOfFinishedSpeech.stop();
        deleteAudioFiles();
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

void AudioRecorder::processBuffer(QAudioBuffer buffer)
{
    QVector<qreal> levels = getBufferLevels(buffer);
    for (int i = 0; i < levels.count(); ++i) {
        if (levels.at(i)*100.0 < thresholdOfAudioinputActivation) {
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
        audioTrigger(true);
    }
}

void AudioRecorder::audioTrigger(bool detectVoice)
{
    if (statusTrigger && !detectVoice) {
        statusTrigger= false;
        audioRecorder->stop();
    }
    if (!statusTrigger && detectVoice) {
        statusTrigger= true;
        setAudioRecorderSettings();
        audioRecorder->record();
        sendStatusProcess("аудиозапись речи");
    }
}

void AudioRecorder::timeoutOfSpeech()
{
    timerOfFinishedSpeech.stop();
    audioTrigger(false);
    if (serviceRecognition == "yandex") {
        audioConversion();
    }
    if (serviceRecognition == "vkAPI") {
        sendPathToAudioFile(filePath);
    }
}

void AudioRecorder::changeServiceRecognition(QString currentService)
{
    serviceRecognition = currentService;
}

void AudioRecorder::setPathToFFMPEG(QString ffmpeg)
{
    audioConversionProcess = ffmpeg.replace(" ", "");
}

void AudioRecorder::audioConversion()
{
    QString pathToAudio;
    QStringList arguments;
    filePath.replace("/", "\\");
    pathToAudio = filePath + ".wav";
    arguments << "-i" << pathToAudio << "-ar" << "48000" << "-t" << "8";
    pathToAudio = filePath + ".ogg";
    arguments << pathToAudio;

    processConvert = new QProcess(this);
    connect(processConvert, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(resultProcess(int, QProcess::ExitStatus)));
    processConvert->start(audioConversionProcess, arguments);
}

void AudioRecorder::resultProcess(int exitCode, QProcess::ExitStatus status)
{
    if (exitCode == 0 && status == QProcess::NormalExit) {
        QFile::remove(fileName + ".wav");
        sendPathToAudioFile(filePath + ".ogg");
    }
    disconnect(processConvert, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(resultProcess(int, QProcess::ExitStatus)));
    delete processConvert;
}

void AudioRecorder::deleteAudioFiles()
{
    for (QString item: listAudioFiles) {
        if (QFile::exists(item + ".wav")) QFile::remove(item + ".wav");
        if (QFile::exists(item + ".ogg")) QFile::remove(item + ".ogg");
    }
    listAudioFiles.clear();
}
