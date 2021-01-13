#include "favitems.h"
#include "ui_favitems.h"
favItems::favItems( widgetGen* wg, QWidget *parent ) :
    QMainWindow( parent ),
    debg(false),
    wgen(wg),
    prev_es(false),
    context(""),
    ui(new Ui::favItems)
{
    ui->setupUi( this );
    setAttribute( Qt::WA_AlwaysStackOnTop, true );
    setAttribute( Qt::WA_NoSystemBackground, true);
    setAttribute( Qt::WA_TranslucentBackground, true );
    setAttribute( Qt::WA_DeleteOnClose, true );
    setAttribute( Qt::WA_TransparentForMouseEvents, false );
}
bool favItems::setContext(QString c){
    wgen->lock();
    if(wgen->setContext(c)){
        /* NEW CONTEXT */
        wgen->setDirector(this);
        wgen->set_markers(&marker_first,&marker_middle,&marker_last,&marker,&marker_switch);
        wgen->setupWrapper({{},{ui->lab_favs_icon,ui->lab_info_items,ui->lab_info_res,ui->lab_info_scenes,ui->lab_info_videos,ui->lab_nick,
                                ui->favs_cnt},{ui->switch_b_frame},{}});
        wgen->setFrames({ui->switch_b_frame_top},{ui->frame});
    }
}
void favItems::init(){
    wgen->setupTopMenuSwitcher(ui->switch_b_frame_top->geometry());
    wgen->hideRastered(ui->centralwidget);
    setMarkers();
}
void favItems::reinit(){
    qDebug()<<"Reinit:"<<context;
    reinit(context,prev_es);
}
void favItems::reinit( QString o, bool es ){
    if(!setContext(o)){
        qDebug()<<"New context:"<<wgen->getContext();
    }
    prev_es=es;
    wgen->showSelectors();
    search_active=es;
    if(search_active){
        wgen->glcontainer->make_edit();
    } else wgen->glcontainer->disableSearch();
}
void favItems::hideEvent( QHideEvent*e){
    e->accept();
    wgen->lock();
}
void favItems::mouseMoveEvent( QMouseEvent*e ){
    wgen->mouseMove(e);
}
void favItems::mousePressEvent( QMouseEvent *e ){
    wgen->mousePress(e);
}
void favItems::mouseReleaseEvent( QMouseEvent *e ){
    wgen->mouseRelease(e);
}
void favItems::create_fvideos_list(int* cnt3,const QJsonArray* fav_titles){
    int cfm=fav_titles->count();
    for(int i=0;i<cfm;i++){
        QJsonObject obj=fav_titles->at(i).toObject();
        QStringList imData=wgen->getThumbRef(obj.value("filename").toString(),obj.value("title").toString());;
        wgen->make_brick( i,
                          obj.value("id").toString().toInt(),
                          obj.value("title_id").toString().toInt(),
                          cnt3[obj.value("title_id").toString().toInt()],
                obj.value("title").toString(),
                QString::number( cnt3[obj.value("title_id").toString().toInt()] )+" items",
                imData.takeFirst(),
                imData.takeFirst(),
                imData.takeFirst(),
                "fav_videos",
                obj.value("oid").toString().toInt());
    }
    wgen->activate(search_active);
    set_favs_cnt(cfm);
}
void favItems::create_videos_list(int* cnt3,const QJsonArray* videos,const QJsonArray* f_titles){
    int cfm=videos->count();
    wgen->set_fav_videos_list(f_titles);
    for(int i=0;i<cfm;i++){
        QJsonObject obj=videos->at(i).toObject();
        QStringList imData=wgen->getThumbRef(obj.value("filename").toString(),  obj.value("title").toString());
        wgen->make_brick(i,
                         obj.value("id").toString().toInt(),
                         obj.value("id").toString().toInt(),
                         cnt3[obj.value("id").toString().toInt()],
                obj.value("title").toString(),
                "",
                imData.takeFirst(),
                imData.takeFirst(),
                imData.takeFirst(),
                "videos",
                obj.value("oid").toString().toInt());
    }
    set_favs_cnt(cfm);
    wgen->activate(search_active);

}
void favItems::create_scenes_list(const QJsonArray* jfs){
    int cfs=jfs->count();
    for(int i=0;i<cfs;i++){
        QJsonObject obj=jfs->at(i).toObject();
        wgen->make_brick( i,
                          obj.value("id").toString().toInt(),
                          obj.value("title_id").toString().toInt(),
                          0,
                          obj.value("title").toString(),
                          obj.value("time").toString(),
                          obj.value("frame").toString(),
                          d_carousel+QUrl(obj.value("frame").toString()).fileName(),
                          serverRoot+obj.value("filename").toString(),
                          "fav_scenes",
                          obj.value("oid").toString().toInt());
        wgen->events[i]=obj.value("event").toString();
    }
    set_favs_cnt(cfs);
    wgen->activate(search_active);
}
void favItems::create_items_list(const QJsonArray* jfi){
    int cfi=jfi->count();
    for(int i=0;i<cfi;i++){
        QJsonObject obj=jfi->at(i).toObject();
        wgen->make_brick( i,
                          obj.value("id").toString().toInt(),
                          obj.value("title_id").toString().toInt(),
                          0,
                          obj.value("name").toString()+"\n from \n "+obj.value("title").toString(),
                          "Duration:"+obj.value("time").toString(),
                          obj.value("image").toString(),
                          d_carousel+QUrl( obj.value("image").toString() ).fileName(),
                          "","fav_items",
                          obj.value("oid").toString().toInt());
        wgen->events[i]=obj.value("event").toString();
    }
    set_favs_cnt(cfi);
    wgen->activate(search_active);
}
void favItems::toggleSelectors(bool v){
    ui->b_scenes_2->setEnabled( v );
    ui->b_items_2->setEnabled( v );
    ui->b_videos_2->setEnabled( v );
    ui->b_scenes->setEnabled( v );
    ui->b_items->setEnabled( v );
    ui->b_videos->setEnabled( v );
}
void favItems::updateUi(){
    ui->b_scenes_2->setVisible( false );
    ui->b_items_2->setVisible( false );
    ui->b_videos_2->setVisible( false );
    ui->b_scenes->setVisible( false );
    ui->b_items->setVisible( false );
    ui->b_videos->setVisible( false );
    ui->lab_info_items->setVisible(false);
    ui->lab_info_scenes->setVisible(false);
    ui->lab_info_videos->setVisible(false);
    ui->lab_info_res->setVisible(false);
    ui->switch_b_frame_top->setVisible(false);
    ui->switch_b_frame->setVisible(false);
    ui->frame->setVisible(false);
    ui->lab_favs_icon->setVisible(false);
    ui->favs_cnt->setVisible(false);
    bool ai = context  == "fav_items";
    bool as = context  == "fav_scenes";
    bool av = context  == "fav_videos";
    bool v = context  == "videos";
    toggleSelectors(!v);
    QString lab;
    ui->lab_info_items->setDisabled(true);
    ui->lab_info_scenes->setDisabled(true);
    ui->lab_info_videos->setDisabled(true);
    ui->lab_info_res->setDisabled(true);
    if( ai ) ui->lab_info_items->setDisabled(false);
    if( as ) ui->lab_info_scenes->setDisabled(false);
    if( av ) ui->lab_info_videos->setDisabled(false);
    if( v ) ui->lab_info_res->setDisabled(false);

    ui->lab_nick->setText(lab);
    if(context!="videos"){
        ui->b_scenes->setStyleSheet( states[as] );
        ui->b_scenes_2->setStyleSheet( states[as] );
        ui->b_scenes->setIcon( wgen->star[as] );
        ui->b_scenes_2->setIcon( wgen->star[as] );
        ui->b_items->setStyleSheet( states[ai] );
        ui->b_items_2->setStyleSheet( states[ai] );
        ui->b_items->setIcon( wgen->star[ai] );
        ui->b_items_2->setIcon( wgen->star[ai] );
        ui->b_videos->setStyleSheet( states[av] );
        ui->b_videos_2->setStyleSheet( states[av] );
        ui->b_videos->setIcon( wgen->star[av] );
        ui->b_videos_2->setIcon( wgen->star[av] );
    }
    ui->favs_cnt->update();
    update();
}
void favItems::setMarkers(){
    marker.move(mapFromGlobal(QPoint(0,0)));
    marker_first.move(mapFromGlobal(ui->switch_b_frame->mapToGlobal(QPoint(0,0)))+QPoint(0,ui->switch_b_frame->height()));
    marker_middle.move(ui->switch_b_frame_top->geometry().bottomLeft());
    marker_switch.move(ui->switch_b_frame_top->geometry().bottomLeft());
    marker_last.move(rect().bottomLeft());
    marker.setFixedSize(0,0);
    marker_first.setFixedSize(0,0);
    marker_middle.setFixedSize(0,0);
    marker_last.setFixedSize(0,0);
}
void favItems::set_favs_cnt( int i ){
    ui->favs_cnt->setText(QString::number(i));
    update();
    wgen->glcontainer->update();
    qDebug()<<"favs cnt:"<<ui->favs_cnt;
}
void favItems::set_nick( QString ni ){
    ui->lab_nick->setText( ni );
    nick = ni;
    wgen->glcontainer->update();
}

/* UI BUTTONS MANAGEMENT*/
void favItems::on_b_items_clicked(){
    if( context  == "fav_items")return;
    emit go( "fav_items",{} );
}
void favItems::on_b_scenes_clicked(){
    if(  context  == "fav_scenes")return;
    emit go( "fav_scenes",{} );
}
void favItems::on_b_videos_clicked(){
    if( context  == "fav_videos")return;
    emit go( "fav_videos",{} );
}
void favItems::on_b_back_clicked(){
    emit go( "back",{} );
}
void favItems::on_b_options_clicked(){
    emit show_menu( ui->b_options->mapToGlobal( QPoint( 0, 0 ) ) );
}
void favItems::on_b_options_2_clicked(){
    ui->b_options->click();
}
void favItems::on_b_back_2_clicked(){
    ui->b_back->click();
}
void favItems::on_b_items_2_clicked(){
    ui->b_items->click();
}
void favItems::on_b_scenes_2_clicked(){
    ui->b_scenes->click();
}
void favItems::on_b_videos_2_clicked(){
    ui->b_videos->click();
}
favItems::~favItems(){
    if(future.isRunning())future.waitForFinished();
    qDebug()<<"DELETING FAVS";
    delete ui;
}
