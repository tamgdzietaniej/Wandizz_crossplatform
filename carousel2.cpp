#include "carousel2.h"
#include "ui_carousel2.h"
Carousel::Carousel(QWidget *parent) :
    QMainWindow (parent),
    debg(false),
    ui(new Ui::Carousel),
    events_counter(-1),
    is_online(true),
    loading(false),
    render_video(false),
    joinsliders(false),
    corr_offset(0),
    old_pos(-1)
{
    ui->setupUi(this);
 //   setAttribute( Qt::WA_AlwaysStackOnTop, true );
    setAttribute( Qt::WA_NoSystemBackground, true);
    setAttribute( Qt::WA_TranslucentBackground, true );
   // setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute( Qt::WA_DeleteOnClose, true );
    setAttribute( Qt::WA_TransparentForMouseEvents, false );
    //  setAttribute( Qt::WA_TransparentForMouseEvents, false );
    man=new QNetworkAccessManager(this);
    connect(man,SIGNAL(finished(QNetworkReply*)),this,SLOT(parser(QNetworkReply*)));
    ui->b_back->setEnabled(true);
}
void Carousel::closeWin(){

}
void Carousel::set_sliders(){
    if(debg)qDebug()<<"mainwidget construct slider";
    sceneslider=new MainWidget(0,this);
    if(debg)qDebug()<<"mainwidget construct itemr";
    itemslider=new MainWidget(1,this);
    sceneslider->setGeometry(ui->sceneframe->geometry());
    itemslider->setGeometry(ui->itemframe->geometry());
    itemslider->setObjectName("itemframe");
    sceneslider->setObjectName("sceneframe");
    setTitle(title);
    setTitle2(names[0]);
    ssize=QSize(sceneslider->geometry().height()*1.76,sceneslider->geometry().height());
    isize=QSize(itemslider->geometry().height()+2,itemslider->geometry().height());
    sceneslider->wsize=ssize;
    itemslider->wsize=isize;
    connect(itemslider,SIGNAL(go(QString,QStringList)),this,SIGNAL(go(QString,QStringList)));
    connect(itemslider,SIGNAL(spin_friend(int)),this,SLOT(synchronize_scene_to_item(int)));
    connect(sceneslider,SIGNAL(spin_friend(int)),this,SLOT(synchronize_item_to_scene(int)));
    connect(itemslider,SIGNAL(add_fav_click()),this,SLOT(add_fav_item_clicked()));
    connect(sceneslider,SIGNAL(add_fav_click()),this,SLOT(add_fav_scene_clicked()));
    connect(this,SIGNAL(can_process()),this,SLOT(roll_scene()),Qt::QueuedConnection);
    //  connect(itemslider,SIGNAL(stop_p()),sceneslider,SLOT(stop()));
    // connect(sceneslider,SIGNAL(stop_p()),sceneslider,SLOT(stop()));

    ui->shadow_lower_2->move(0,sceneslider->geometry().bottom()+1);
    ui->shadow_lower_3->move(0,itemslider->geometry().bottom()+1);
}
void Carousel::reinit(){
    not_loaded_yet=true;
    cnt=0;
    show_yt=false;
    pressed=false;
    trip_to=-1;
    setTitle();
    loading=true;

}

void Carousel::set_params(QString tit,int tid,QString nf,const int offs_ms,int pl_offset,bool pla_en){
    if(debg)qDebug()<<"CAROUSEL: SET PARAMS:"<<pl_offset<<tid;
    title=tit;
    setTitle(title);
    paused_time=offs_ms;
    netfile=nf;
    title_id=tid;
    sceneslider->player_offset=pl_offset;
    sceneslider->shplayer=pla_en;
    sceneslider->init_player(netfile);
    if(paused_time==0){
        sceneslider->slave=true;
        itemslider->slave=false;
    } else {
        sceneslider->slave=false;
        itemslider->slave=true;
    }
    reinit();
}
void Carousel::updateMeta(){
    if(debg)if(debg)qDebug()<<"WAND:CAROUSEL:PAUSED:"<<paused_time;
    int _st_id=-1;
    int _time;
    int _st=duration+1;
    qint64 _diff;
    int _st_time=duration;
    setTitle(title);
    not_loaded_yet=true;
    for(int i=0; i<events_counter;i++){
        _time=ev_data[i].etime;
        _name=ev_data[i].name;
        _event=ev_data[i].event;
        _image=ev_data[i].image;
        _dur=ev_data[i].dur;
        _diff=_time-paused_time;
        if(abs(_diff)<=abs(_st)){
            _st=_diff;
            _st_id=i;
            _st_time=_time;
        }
        if(_diff >= -500 && _diff<500){
            if(last_event!=_name){
                last_event=_name;
                not_loaded_yet=false;
                if(itemslider->slave){
                    not_loaded_yet=false;
                    roll_to(i,0);
                } else roll_to(i,1);
                if(i==events_counter){
                    setTitle("THE END");
                } else {
                    roll_to(i,1);
                }
            }
        }
    }
    if(not_loaded_yet){
        setTitle(title);
        roll_to(_st_id,0);
        setTitle2(ev_data[_st_id].name+" for: "+QString::number(round(_st/1000))+" secs");
        paused_time=_st_time;
    }
}
void Carousel::aimParser(){
    if(show_yt)return;
    curr_title=title;
    QString ctitle=QUrl::toPercentEncoding(title);
    QByteArray wdz;
    QString wdzsrc=get_wdz_url+ctitle;
    QNetworkRequest request;
    request.setUrl(QUrl(wdzsrc));
    man->get(request);
}
void Carousel::parser(QNetworkReply* reply){
    origin_pos=0;
    QString p=reply->readAll();
    reply->deleteLater();
    empties=0;
    QString _image_root,p_time,p_event,p_image,p_frame,p_ev_name;
    if(p!=""){
        p.replace("<hr>","",Qt::CaseInsensitive);
        QStringList plines=p.split("\n");
        plines.removeLast();
        duration=plines.takeLast().toInt();
        int all=plines.count()*2;
        if(all==0){
            qDebug()<<"NO ITEMS";
            emit cant_process();
        }
        QTime _tmp_t(0,0,0);
        ui->end_lab->setText(ms2hms(duration));
        setTitle(title);
        empties=0;
        QString ctitle=title;
        ctitle.replace(" ","_");
        int checkpoint_ms=0;
        int offset_ms=0;
        for(int j=0;j<all/2;j++){
            QStringList dta=plines[j].split(";");
            // faulty wdz line
            if(dta.count()<8){
                empties++;
                continue;
            } else {
                events_counter++;
                dta[4].replace(" ","%20");
                dta[3].replace(" ","%20");
                checkpoint_ms=hms2ms(dta[0]);
                offset_ms=hms2ms(dta[1]);
                ev_data[events_counter].event=dta[5];
                ev_data[events_counter].etime=checkpoint_ms;
                ev_data[events_counter].image=serverRoot+dta[4];
                ev_data[events_counter].dur=offset_ms;
                ev_data[events_counter].scene_id=dta[2].toInt();
                ev_data[events_counter].frame=serverRoot+dta[3];
                ev_data[events_counter].name=dta[6];
                ev_data[events_counter].item_id=dta[7].toInt();
                ev_data[events_counter].tline_id=dta[8].toInt();
                ev_data[events_counter].item_file=d_carousel+QUrl(dta[4]).fileName();
                ev_data[events_counter].frame_file=d_carousel+QUrl(dta[3]).fileName();
            }
        }
        int closest_origin=duration;
        events_counter++;
        for(int i=0; i<events_counter;i++){
            int _t=ev_data[i].etime;
            qint64 _diff=_t-paused_time;
            if(_diff>duration)origin_pos=itemslider->last_created;
            else {
                if(closest_origin>qAbs(_diff)){
                    closest_origin=qAbs(_diff);
                    origin_pos=i;
                }
            }
        }
    }
        generate_widget();
    qDebug()<<"CAROUSEL FAVS:"<<fav_items_list<<fav_scenes_list;
    itemslider->prepare(ev_data,events_counter,isize,netfile,fav_items_list);
    sceneslider->prepare(ev_data, events_counter,ssize,netfile,fav_scenes_list);

    irequested.clear();
    srequested.clear();
    wind=0;
    first_time=true;
    if(origin_pos<0)origin_pos=0;
    emit can_process();
}
void Carousel::generate_widget(){

    //future = QtConcurrent::run([=]() {
        for(int wind=0;wind<events_counter;wind++){
            if(exiting){
                break;
            }
            if(!srequested.contains(ev_data[wind].scene_id)){
                if(!QFileInfo::exists(ev_data[wind].frame_file)){
                    srequested.append(ev_data[wind].scene_id);

                    emit download(ev_data[wind].frame,ev_data[wind].frame_file);

                }
            }
            if(!irequested.contains(ev_data[wind].item_id)){
                if(!QFileInfo::exists(ev_data[wind].item_file)){
                    irequested.append(ev_data[wind].item_id);

                    emit download(ev_data[wind].image,ev_data[wind].item_file);
                }
            }
        }

 //   });
}
void Carousel::can_proc(){
    qDebug()<<"CAN PROCESS";
    emit can_process();
}
void Carousel::roll_scene(){
    if(first_time){
        first_time=false;
        //   QApplication::processEvents();
        update();
        sceneslider->update();
        itemslider->update();
        if(debg)qDebug()<<"start scenesliders timers";
        sceneslider->start_bg_proc();
        itemslider->start_bg_proc();
        roll_to(origin_pos,0);
    }
}
bool Carousel::roll_to(int rt,int target){
    bool r;
    if(rt<0)rt=0;
    set_curr_data(rt);
    setTitle2(ev_data[rt].name);
    if(target==0){
        r=sceneslider->set_trip_to(rt);
    } else {
        r=itemslider->set_trip_to(rt);
    }
    if(!slider_pressed)ui->duration->setValue((ev_data[rt].etime*100)/duration);
    return r;
}

// DISPLAY MESSAGES, TITLES AND OTHER SHIT
void Carousel::setTitle(const QString title){
    ui->title->setText(title);
    ui->title->setAlignment(Qt::AlignCenter);
    ui->title->update();
    ui->b_back->raise();
}
void Carousel::setTitle(){
    ui->title->clear();
    ui->b_back->raise();
}
void Carousel::on_duration_valueChanged(int val){
    qreal offset_ms=(qreal(val)/100)*duration;
    QString hms=ms2hms(offset_ms);
    ui->time_lab->setText(hms);
}
void Carousel::on_duration_sliderMoved(int pos){
    if(slider_pressed){
        itemslider->set_slave();
        sceneslider->unset_slave();

    }
    paused_time=(qreal(pos)/100)*duration;
    updateMeta();
}
void Carousel::deb(QString d){
    emit sdebg(d);
}
void Carousel::setTitle2(const QString title){
    ui->title2->setText(title);
}
void Carousel::setTitle2(){
    ui->title2->clear();
}
void Carousel::set_curr_data(int i){
    faced_item_data=ev_data[i];
    faced_scene_data=ev_data[i];
}
void Carousel::synchronize_scene_to_item(int i){
    // if(debg)qDebug()<<"SSI"<<i;
    sceneslider->set_slave();
    sceneslider->stop();
    setTitle2(faced_item_data.name);
    roll_to(i,0);
}
void Carousel::synchronize_item_to_scene(int s){
    //   if(debg)qDebug()<<"SIS"<<s;
    itemslider->set_slave();
    int fa=sceneslider->first_assigned[s];
    int la=sceneslider->last_assigned[s];
    if(la<itemslider->mw) s=la;
    else  s=fa;
    setTitle2(faced_scene_data.name);
    roll_to(s,1);
}
QString Carousel::ms2hms(int _msecs){
    QTime _tmp_t(0,0,0);
    return   _tmp_t.addMSecs(_msecs).toString("hh:mm:ss");
}
int Carousel::hms2ms(QString hms){
    hms=hms.replace(".",":");
    QStringList _tmp=hms.split(":");
    int msecs=0;
    if(_tmp.length()==4)msecs=_tmp[0].toInt()*3600000+_tmp[1].toInt()*60000+_tmp[2].toInt()*1000+_tmp[3].toInt();
    else if(_tmp.length()==3)msecs=_tmp[0].toInt()*3600000+_tmp[1].toInt()*60000+_tmp[2].toInt()*1000;
    else   qCritical()<<"WRONG TIME FORMAT:"<<hms;
    return msecs;
}
void Carousel::check_mw_fav(){
    sceneslider->slide[sceneslider->mw].is_fav=is_scene_fav(sceneslider->mw);
    itemslider->slide[itemslider->mw].is_fav=is_item_fav(itemslider->mw);
}
void Carousel::add_fav_scene_clicked(){
    if(debg)qDebug()<<"WAND:CAROUSEL:ADD FAV SC CLICK"<<sceneslider->mw;
    faced_scene_data=sceneslider->ev_data[sceneslider->mw];
    int sid=faced_scene_data.scene_id;
    bool isfav=is_scene_fav(sceneslider->mw);
    if(isfav){
        if(debg)qDebug()<<"WAND:CAROUSEL:DELETING SCENE:"<<sceneslider->mw;
        fav_scenes_list.removeAt(fav_scenes_list.indexOf(sid));
        emit del_fav("favscenes",sid);
    } else {
        if(debg)qDebug()<<"WAND:CAROUSEL:ADDING SCENE:"<<sceneslider->mw;
        fav_scenes_list.append(faced_scene_data.scene_id);
        emit add_fav_scene(sid);
    }
    if(debg)qDebug()<<"WAND:CAROUSEL:ADD FAV SC CLICK AFTER";
}
void Carousel::add_fav_item_clicked(){
    if(debg)qDebug()<<"CAROUSEL:"<<itemslider->mw;
    faced_item_data=itemslider->ev_data[itemslider->mw];
    int iid=faced_item_data.item_id;

    int isfav=is_item_fav(itemslider->mw);
    if(isfav){
        if(debg)qDebug()<<"WAND:CAROUSEL:DELETING ITEM:"<<itemslider->mw<<fav_items_list;
        fav_items_list.removeAt(fav_items_list.indexOf(iid));
        emit del_fav("favitems",faced_item_data.item_id);
    } else {
        if(debg)qDebug()<<"WAND:CAROUSEL:ADDING ITEM:"<<itemslider->mw<<fav_items_list;
        fav_items_list.append(faced_item_data.item_id);
        emit add_fav_item(faced_item_data.item_id);
    }
    if(debg)qDebug()<<"WAND:CAROUSEL:SET FAV:"<<itemslider->mw;
}
void Carousel::on_b_back_clicked(){
    qDebug()<<"WAND:CAROUSEL:back:";
    emit go("back",{});
}
void Carousel::on_duration_sliderReleased(){
    slider_pressed=false;
}
void Carousel::on_b_options_clicked(){
    emit go("menu",{});
}
void Carousel::on_b_options_pressed(){
    old_ss=ui->b_options->styleSheet();
    ui->b_options->setStyleSheet(old_ss+";color:orange");
}
void Carousel::on_b_options_released(){
    ui->b_options->setStyleSheet(old_ss);

}
void Carousel::set_fav_items_list(const QJsonArray* doc){
    int _cnt=doc->count();
    fav_items_list.clear();
    for(int i=0;i<_cnt;i++){
        qDebug()<<"CAR FAV:"<<doc->at(i).toObject().value("fav_id").toString().toInt();
        fav_items_list.append(doc->at(i).toObject().value("fav_id").toString().toInt());
    }
}
void Carousel::set_fav_scenes_list(const QJsonArray* doc){
    qDebug()<<"FAV SCENES:"<<doc->toVariantList();
    int _cnt=doc->count();
    fav_scenes_list.clear();
    for(int i=0;i<_cnt;i++){
        fav_scenes_list.append(doc->at(i).toObject().value("fav_id").toString().toInt());
        qDebug()<<"SCFAV:"<<(doc->at(i).toObject().value("fav_id").toString().toInt());
        fav_items_list.append(doc->at(i).toObject().value("fav_id").toString().toInt());
    }
}
void Carousel::set_scene_fav(int i){
    //  sceneslider->slide[sceneslider->cluster[i]]->set_fav(is_scene_fav(i));
}
void Carousel::set_item_fav(int i){
    //    itemslider->slide[i]->set_fav(is_item_fav(i));
}
bool Carousel::is_scene_fav(int i=-1){
    if(i==-1)i=sceneslider->mw;
    int si=sceneslider->ev_data[i].scene_id;
    qDebug()<<"CHECK SCENE"<<si<<ev_data[i].scene_id;
    return fav_scenes_list.contains(si);
}
bool Carousel::is_item_fav(int i=-1){
    if(i==-1)i=itemslider->mw;
    if(debg)qDebug()<<"WAND:CAROUSEL:CHECK IF ITEM FAV:"<<fav_items_list<<"-----------"<<itemslider->ev_data[i].item_id<<fav_items_list.contains(itemslider->ev_data[i].item_id);
    qDebug()<<"FAVITS:"<<fav_items_list;
    return fav_items_list.contains(itemslider->ev_data[i].item_id);
}
void Carousel::showEvent(QShowEvent* e){
    setDisabled(false);
    sceneslider->lock=false;
    itemslider->lock=false;
    sceneslider->timer->start();
    itemslider->timer->start();
    e->accept();
}
void Carousel::hideEvent(QHideEvent* e){
    sceneslider->lock=true;
    itemslider->lock=true;
    sceneslider->timer->stop();
    itemslider->timer->stop();
    e->accept();
}
void Carousel::perform_close(){
    sceneslider->perform_close();
    close();
}
Carousel::~Carousel(){
    //   if(debg)qDebug()<<"WAND:CAROUSEL:destruktor CAROUSEL";
    exiting=true;
    future.cancel();
    future.waitForFinished();
    delete ui;
}
void Carousel::on_duration_sliderPressed(){
    qDebug()<<"Destructor carousel";
    watcher.cancel();
    watcher.waitForFinished();
    slider_pressed=true;
}

