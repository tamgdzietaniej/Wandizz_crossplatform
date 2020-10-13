#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include "globals.h"

/* CONSTRUCTOR */
class AudioRecorder : public QObject
{
    Q_OBJECT

    /* PUBLIC */
public:
    explicit AudioRecorder(QObject *parent = nullptr);
    ~AudioRecorder();
    QAudioEncoderSettings settings;
    QAudioRecorder* recorder;
    bool is_recording;
    QString container,fnam;
    /* SLOTS */
public slots:
    void RecorderSetup();
    void record();
    void start();
    void stop();
signals:
    void finished();
    void isRecording();
    void isStopped();
};

#endif // AUDIORECORDER_H
