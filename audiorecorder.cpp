#include "audiorecorder.h"

AudioRecorder::AudioRecorder(QObject *parent) : QObject(parent){
    recorder=new QAudioRecorder();
    is_recording=false;
    RecorderSetup();
}
void AudioRecorder::RecorderSetup(){
    QString codec;
    codec=recorder->supportedAudioCodecs().first();
    container=recorder->supportedContainers().first();
    settings.setSampleRate(48000);
    settings.setBitRate(128000);
    settings.setCodec(codec);
    recorder->setContainerFormat(container);
    recorder->setAudioSettings(settings);
    fnam=f_recorder+".wav";
    qDebug()<<"CONTAINER:"<<fnam;
    QFile* tmp=new QFile(fnam);
    tmp->open(QIODevice::WriteOnly);
    if(!tmp->isOpen()){
        qDebug() << "TEMPORARY videos FILE NOT OPENED!!!";
    }
    tmp->close();
    recorder->setOutputLocation(QUrl::fromLocalFile(fnam));
    qDebug()<<"AVAIL.CODECS:"<<recorder->supportedAudioCodecs()<<"("<<(recorder->supportedAudioCodecs()).length()<<")";;
    qDebug()<<"AVAIL.CONTAINERS:"<<recorder->supportedContainers()<<"("<<(recorder->supportedContainers()).length()<<")";
    qDebug()<<"------------------------";
    qDebug()<<"AUDIORECORDER SETTINGS:";
    qDebug()<<"CONTAINER:"<<recorder->containerFormat();
    qDebug()<<"CODEC: " << codec;
    qDebug()<<"CODEC DESCRITION:"<<recorder->audioCodecDescription(codec);
    qDebug()<<"CONTAINER DESCRITION:"<<recorder->containerDescription(container);
    //   qDebug()<<"AUDIO INPUT:"<<recorder->audioInputs()<<"["<<recorder->defaultAudioInput()<<"]";
    //   qDebug()<<"SAMPLE RATES:"<<recorder->supportedAudioSampleRates();
    qDebug()<<"OUt file:"<<recorder->actualLocation();
    //   qDebug()<<"SAMPLE RATE:"<<recorder->audioSettings().sampleRate();
    //   qDebug()<<"INPUt:"<<recorder->audioInputs()<<"."<<recorder->audioInput()<<"."<<recorder->defaultAudioInput();
    //   qDebug() << "DESTINATION FOR SAMPLING: " << recorder->actualLocation()<<"[" <<dest_file<<"]";
}
void AudioRecorder::stop(){
    recorder->stop();
}
void AudioRecorder::start(){
    recorder->stop();
    recorder->record();
}
void AudioRecorder::record(){
    if(is_recording){
        is_recording=false;
        recorder->stop();
    } else{
        is_recording=true;
        recorder->record();
    }
}
AudioRecorder::~AudioRecorder(){
    emit finished();
    qDebug()<<"~AudioRecorder()";

}

