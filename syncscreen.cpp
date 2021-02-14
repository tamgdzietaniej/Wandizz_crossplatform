#include "syncscreen.h"
#include "ui_syncscreen.h"
SyncScreen::SyncScreen(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SyncScreen)
{
    ui->setupUi(this);
    ui->record->setIdleColor(ui->record->color_title);
    ui->record->setVisible(true);
    ui->record->setFocus();
    ui->record->raise();
    qDebug()<<"UI:"<<ui->record->geometry();
    ui->youtube->setVisible(false);
    attempt=0;
    manual_stop=false;
    test=false;
    oldsadj=0;
    car_sh=false;
    gradient2.setCoordinateMode(QLinearGradient::ObjectMode);
    gradient2.setStart(0,0);
    gradient2.setFinalStop(QPoint(0,1));
    gradient2.setColorAt(0, QColor(0,0,0,150));
    gradient2.setColorAt(0.5, QColor(0,0,0,120));
    gradient2.setColorAt(1, QColor(0,0,0,150));
    setAutoFillBackground(false);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_NoSystemBackground,false);
    setAttribute(Qt::WA_TranslucentBackground,true);
    setAttribute(Qt::WA_OpaquePaintEvent,true);

}
/*
void SyncScreen::paintEvent(QPaintEvent *){
    qDebug()<<"paint geo:"<<geometry();
    QPainter painter(this);
    painter.setPen(QColor(0,0,0,0));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::NonCosmeticDefaultPen);
    painter.fillRect(geometry(),QBrush(gradient));
      //  qDebug()<<"SHADOW:"<<QRect(15*width()/400,20*height()/710,width()-30*width()/400,height()-40*height()/710);
        painter.setBrush(gradient2);
        painter.drawRoundedRect(QRect(15*width()/400,20*height()/710,width()-30*width()/400,height()-40*height()/710),3,3);
    painter.end();
      qDebug()<<"NAV:Paintgeo end";
}
*/
void SyncScreen::initClasses(){

    // RECORDER AND THREAD
    initTimers();
    recorder=new AudioRecorder(this);
    connect(recorder->recorder,SIGNAL(stateChanged(QMediaRecorder::State)),this,SLOT(status(QMediaRecorder::State)),Qt::QueuedConnection);

    ui->record->set_geo();
    m_probe = new QAudioProbe(this);
    connect(m_probe, &QAudioProbe::audioBufferProbed,
            this, &SyncScreen::processBuffer);
    m_probe->setSource(recorder->recorder);
    //amazon=new aws_api();
    acr=new acr_api(this);
     connect(acr->networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getAnswer(QNetworkReply*)),Qt::ConnectionType::DirectConnection);
    sanim=new QPropertyAnimation(ui->circless,"geometry",this);
    sanim->setEasingCurve(QEasingCurve::Linear);
    sanim->setDuration(100);
    sanim->setDirection(QPropertyAnimation::Forward);
    srect=ui->circless->geometry();
}

/*
 *  CLIENT UPLOAD
 *
 */
void SyncScreen::on_send_cli_clicked()
{
    acr->sendFile(recorder->fnam,"audio","/v1/identify");
}

/*
 *  CLIENT ACR API ANSWER PARSER
 *
 */
void SyncScreen::getAnswer(QNetworkReply* reply){
    emit start_offset_meter();
    ui->record->setProgress(0);
    if(reply->bytesAvailable()>0){
        found=false;
        QString response=reply->readAll();
        /*
        downloaderStd* downloader=new downloaderStd();
        QString update_user_activity = "http://wandizz.com/acr/api/checkSigned.php?user_activity=got_response&curr_id="+curr_id+"&payload="+curr_payload+"&response="+response+coords;
        qDebug()<<"SQL:"+update_user_activity;
        qDebug()<<"COORDS:"<<coords;
        curr_primary_key_id=downloader->download(update_user_activity);
        downloader->deleteLater();
*/
        QString artist,stat;
        QJsonArray  music_array, custom_array, artists;
        QJsonObject music_report,custom_report, album_case, metadata;
        QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
        QJsonObject status(doc["status"].toObject());
        int loop, inner_loop1;
        int c=0;
        qDebug().noquote() << "STAT";
        stat=status["msg"].toString();
        qDebug()<<"C"<<c++;
        metadata=doc["metadata"].toObject();
        qDebug()<<"C"<<c++;
        qDebug().noquote() << response;
        content_type="none";
        if(stat=="Success"){
                qDebug()<<"C"<<c++;
            found=true;
                    qDebug()<<"C3"<<c++;
            ui->record->setState(ui->record->color_success,1000);
            //event_file="no_event_file_action.txt";
                    qDebug()<<"C4"<<c++;
            if(metadata.contains("music")){
                qDebug()<<"C"<<c++;
                content_type="Music";
                music_array=metadata["music"].toArray();
          qDebug()<<"CL"<<c++;
                for(loop=0;loop<music_array.size();loop++){
                    music_report=music_array.at(loop).toObject();
                    artists=music_report["artists"].toArray();
                    for(inner_loop1=0;inner_loop1<artists.size();inner_loop1++){
                        artist=(artists.at(inner_loop1).toObject())["name"].toString();
                    }
                    //  QString dur=music_report["duration_ms"].toInt();
                    //  qint64 duration=dur.toLong();
                    duration=music_report["duration_ms"].toInt();
                    if(duration==0)duration=1;
                    album_case= music_report["album"].toObject();
                    offset_ms=music_report["play_offset_ms"].toInt();
                    album= album_case["name"].toString();
                    title=music_report["title"].toString();
                    artist_and_album="artist="+QUrl::toPercentEncoding(artist)+
                                       "&title="+QUrl::toPercentEncoding(title)+"&album="+QUrl::toPercentEncoding(album)+
                                       "&content-type="+content_type;
                    //artist_and_album=artist+" "+album;
                    setTitle2(artist.replace("+"," ")+", "+title);
                    int calc_off=offset_ms/1000;
                    int off_h=calc_off/3600;
                    QString offset_str=QString::number(off_h)+":";
                    calc_off-=off_h*3600;
                    int off_m=calc_off/60;
                    offset_str+=QString::number(off_m)+":";
                    calc_off-=off_m*60;
                    int off_s=calc_off;
                    offset_str+=QString::number(off_s);
                    browser_data=eventer_query+"?"+artist_and_album+"&curr_id="+curr_primary_key_id;
                    qDebug()<<"BROWSER:"<<browser_data;
                    attempt=0;
                    ui->youtube->setVisible(true);
                }
            }
            else
                if(metadata.contains("custom_files")){
                custom_array=metadata["custom_files"].toArray();
                for(loop=0;loop<custom_array.size();loop++){
                    custom_report=custom_array.at(loop).toObject();
                    title=custom_report["title"].toString();
                    offset_ms=custom_report["play_offset_ms"].toInt();
                    QString duration_ms_str=custom_report["duration_ms"].toString();
                    duration=duration_ms_str.toLong();
                    if(custom_report.contains("content_type")){
                        content_type=custom_report["content_type"].toString();
                    }
                    /*
                    if(custom_report.contains("event_file")){
                        event_file=custom_report["event_file"].toString();
                    } else {
                        artist_and_album=title;
                    }
                    */
                    int calc_off=offset_ms/1000;
                    int off_h=calc_off/3600;
                    setTitle2(title);
                    QString offset_str=QString::number(off_h)+":";
                    calc_off=offset_ms/1000-off_h*3600;
                    int off_m=calc_off/60;
                    offset_str+=QString::number(off_m)+":";
                    calc_off=offset_ms/1000-off_h*3600-off_m*60;
                    int off_s=calc_off;
                    offset_str+=QString::number(off_s);
                    attempt=0;

                    carousel(offset_str);
                }
            }

            else {
                qDebug() << "PARSER: API: Format jeszcze nie obsługiwany";
            }
        } else {
            int error_code=status["code"].toInt();
            qDebug() << "Error: " << error_code;
            if(error_code==3015){
                qDebug() << "Too many request :(";
                setTitle2("!too many requests!");
                if(error_code==3015)qDebug() << "Nieznane :(";
                if(error_code==1001){
                    setTitle2("sorry, cannot recognize.");
                }
            }
        }
        qDebug()<<show_yt;
    }
    if(!found){
        ui->record->setState(ui->record->color_error,1000);
        if(attempt<3){
            if(!manual_stop)recorder->start();
            else{
                attempt=0;
            }
        } else {
            attempt=0;


        }

    }
}

void SyncScreen::setTitle2(const QString title){
    ui->title2->setVisible(true);
    ui->title2->clear();
    ui->title2->setText(title);
    //  ui->title2->setFixedSize(screenw*0.9f,ui->title2->sizeHint().height());
    ui->title2->setAlignment(Qt::AlignCenter);
    ui->title2->updateGeometry();
    ui->title2->update();
}
void SyncScreen::setTitle2(){
    ui->title2->setVisible(true);
    ui->title2->clear();
    ui->title2->update();
}
void SyncScreen::setTitle2(const QString title, int timeout){
    setTitle2(title);
    t2.singleShot(timeout,this,SLOT(fadeOut2()));
}

void SyncScreen::carousel(QString offset){
    emit go("carousel",{title,offset});
}
/**************************************************
 * ********************************************** *
 * *        TIMERS                              * *
 * ************************************************/
void SyncScreen::updateTicker(){
    if(recorder->is_recording)startTicker();
    finfo.refresh();
    double psize=finfo.size()/1000;
    if(psize>1000)recTimeout();
    curr_payload=QString::number(psize);
    if(!manual_stop && autorectime>0){
        ui->record->setProgress((autorectime-autorectimer.remainingTime())*100/autorectime);
    }
    //   record->setText("PAYLOAD: "+curr_payload+"kb.");
}
void SyncScreen::startTicker(){
    ticker->singleShot(100,this, SLOT(updateTicker()));
}
void SyncScreen::stopTicker(){
    ticker->stop();
}

void SyncScreen::initTimers(){
    ticker = new QTimer(this);
    meta_ticker = new QTimer(this);
    synchro_ticker=new QTimer(this);
    fader=new QTimer(this);


}


void SyncScreen::setupEncoder(){
    QString codec=recorder->recorder->supportedAudioCodecs().first();
    QString cont=recorder->recorder->supportedContainers().first();
#if defined(Q_OS_ANDROID)
    QStringList containers=recorder->recorder->supportedContainers();
    QStringList codecs=recorder->recorder->supportedAudioCodecs();
#endif
}

/*
 * - - - - - - - - - - R E C O R D E R - - - - - - - - - -
 *
 *  STATE
 *
 *
 */
void SyncScreen::status(QMediaRecorder::State state){
    switch(state){
    case QMediaRecorder::State::RecordingState:
        //   record->setStyleSheet("background-color:transparent;border:none;border-image: url(:/gui/sync_button_active.png) 0 0 0 0 stretch stretch;");
        isRecording();
        break;
    case QMediaRecorder::State::StoppedState:
        //   record->setStyleSheet("background-color:transparent;border:none;border-image: url(:/gui/sync_button.png) 0 0 0 0 stretch stretch;");
        isStopped();
        break;
    case QMediaRecorder::State::PausedState:
        break;
    default:
        //   qDebug()<<"QMediaRecorder::State::"<<state;
        break;
    }
    qDebug()<<"ERRORSTRING:"<<recorder->recorder->errorString();
}

void SyncScreen::isRecording(){
    attempt++;
    recorder->is_recording=true;
    emit here_i_am();
    autorectime=7000*attempt;
    timer.singleShot(autorectime,this, SLOT(recTimeout()));
    autorectimer.start(autorectime);
    //  record->setChecked(true);
    QString txt[5];
    txt[1]="now, I'm listening.";
    txt[2]="I'm listening again...";
    txt[3]="And last try.";
    setTitle2();
    startTicker();
    finfo.setCaching(true);
    finfo.setFile(f_recorder);
    //  qDebug()<< "PROBING ATTEMPT:"<<QString::number(attempt);

}

/*
 *  IS STOPPED
 *
 */
void SyncScreen::isStopped(){
    //  qDebug() << "isstopped";
    recorder->is_recording=false;
    //stopTicker();

    if(!manual_stop){
        on_send_cli_clicked();
    } else {
        ui->record ->setState(ui->record->color_error,1000);
        setTitle2();
        timer.stop();
        autorectimer.stop();
        ui->record->setProgress(0);
    }
}
/*
 * TIMEOUT
 *
 */
void SyncScreen::recTimeout(){
    qDebug() << "rec timeout";
    if(recorder->is_recording){
        qDebug()<<"recorder is recording and now its gonna stop";
        autorectimer.stop();
        recorder->stop();
    }
}
/*
 * RECORDER BUTTON CLICK USER
 *
 */
void SyncScreen::on_record_clicked(){
    ui->youtube->setVisible(false);
    ui->circless->setMinimumSize(1,1);
    qDebug()<<"recorder:"<<recorder->is_recording;
    if(!recorder->is_recording){
        qDebug() << "RECORDER_START_BY_BUTTON";
        recorder->start();
        manual_stop=false;
    }    else  {
        qDebug() << "RECORDER_STOP_BY_BUTTON";
        //      ui->record->setStyleSheet("background-color:transparent;border:none;border-image: url(:/gui/sync_button.png) 0 0 0 0 stretch stretch;");
        recorder->stop();
        attempt=0;
        manual_stop=true;
    }
}

SyncScreen::~SyncScreen()
{
    delete ui;
    qDebug()<<"~SyncScreen()";
}

void SyncScreen::on_youtube_clicked()
{
    qDebug()<<"SYNC-WWW:"<<browser_data;
    emit(go("web",{browser_data}));
}

void SyncScreen::show_carousel(){
    title=sender()->objectName();
    emit start_offset_meter();
    carousel("00:00:00:000");
    opt_pressed=false;
}

void SyncScreen::on_b_options_clicked()
{
    emit go("menu",{});
}
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

void SyncScreen::processBuffer(const QAudioBuffer& buffer)
{
    QVector<qreal> levels = getBufferLevels(buffer);
    qreal sl;
    for (int i = 0; i < levels.count(); ++i){
        sl=levels.at(i);
 //       qDebug()<<"!!!!!!!!!"
 //                   "!!!!!!! LEVEL:"<<i<<sl-0.5;
    }
    if(sanim->state()==QPropertyAnimation::Running)
        sanim->stop();

    qreal sadj=-sl * ui->circless->width() / 2;
    sanim->setStartValue(ui->circless->geometry());
    sanim->setEndValue(srect.adjusted(-sadj,-sadj,sadj,sadj));
    sanim->start();
   // qDebug()<<"ANIM FROM:"<<ui->circless->geometry()<<"to"<<sanim->endValue();

}
void SyncScreen::clearAudioLevels(){
    for (int i = 0; i < m_audioLevels.size(); ++i)
        m_audioLevels.at(i)->setLevel(0);
}


void SyncScreen::on_b_back_clicked()
{
    emit go("back",{});
}
