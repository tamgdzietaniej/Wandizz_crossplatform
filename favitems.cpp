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
    connect(wg,SIGNAL(fav_update(int)),this,SLOT(set_favs_cnt(int)));
}
bool favItems::setContext(QString c){
    wgen->setupTopMenuSwitcher(ui->switch_b_frame_top->geometry());
    context=c;
    if(wgen->setContext(c)){
        /* NEW CONTEXT */
        wgen->setDirector(this);
        updateUi();
        wgen->glcontainer->setupWrapper({{},{ui->lab_favs_icon,ui->lab_info_items,ui->lab_info_res,ui->lab_info_scenes,ui->lab_info_videos,ui->lab_nick,ui->favs_cnt},{ui->switch_b_frame},{}});
        //    wgen->top_menu_switcher->setFrames(ui->switch_b_frame_top,ui->frame);
    }
    show();
    wgen->glcontainer->show();
    wgen->glcontainer->update();
    wgen->top_menu_switcher->show();
    wgen->top_menu_switcher->update();
    update();
    wgen->start_bg_proc();
}
void favItems::init(){
    wgen->setupTopMenuSwitcher(ui->switch_b_frame_top->geometry());
    corr_y=ui->switch_b_frame->geometry().bottom();
    vcorr=ui->switch_b_frame_top->geometry();
}
void favItems::reinit(){
    qDebug()<<"Reinit:"<<context;
    reinit(context,prev_es);
}
void favItems::reinit( QString o, bool es ){
    wgen->setupTopMenuSwitcher(ui->switch_b_frame_top->geometry());
    wgen->setFrames(ui->switch_b_frame_top,ui->frame);
    setObjectName(o);
    if(!setContext(o)){
        qDebug()<<"New context:"<<wgen->getContext();
    }
    prev_es=es;
    wgen->setMFCorr(vcorr.top(),corr_y);
    wgen->setSelectors(true);
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
        QString its;
        if(cnt3[obj.value("id").toString().toInt()]>0)
            its=QString::number( cnt3[obj.value("title_id").toString().toInt()] )+" items";
        wgen->make_brick( i,
                          obj.value("id").toString().toInt(),
                          obj.value("title_id").toString().toInt(),
                          cnt3[obj.value("title_id").toString().toInt()],
                obj.value("title").toString(),
                its,
                imData.takeFirst(),
                imData.takeFirst(),
                imData.takeFirst(),
                "fav_videos",
                obj.value("oid").toString().toInt());
    }
    wgen->activate(search_active);
    set_favs_cnt(cfm);
}
void favItems::create_videos_list(int* cnt3,const QJsonArray* videos){
    int cfm=videos->count();
    for(int i=0;i<cfm;i++){
        QJsonObject obj=videos->at(i).toObject();
        QStringList imData=wgen->getThumbRef(obj.value("filename").toString(),  obj.value("title").toString());
        QString its;
        if(cnt3[obj.value("id").toString().toInt()]>0)
            its=QString::number( cnt3[obj.value("id").toString().toInt()] )+" items";
        wgen->make_brick(i,
                         obj.value("id").toString().toInt(),
                         obj.value("id").toString().toInt(),
                         cnt3[obj.value("id").toString().toInt()],
                obj.value("title").toString(),
                its,
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
                          obj.value("scene_image").toString(),
                          d_carousel+QUrl(obj.value("scene_image").toString()).fileName(),
                          serverRoot+obj.value("filename").toString(),
                          "fav_scenes",
                          obj.value("oid").toString().toInt(),
                          obj.value("event").toString());
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
                          obj.value("name").toString()+"\n from",
                          obj.value("title").toString(),
                          obj.value("image").toString(),
                          d_carousel+QUrl( obj.value("image").toString() ).fileName(),
                          "","fav_items",
                          obj.value("oid").toString().toInt(),obj.value("event").toString());
    }
    set_favs_cnt(cfi);
    wgen->activate(search_active);
}
void favItems::toggleFavButtons(bool t){
    ui->b_items_2->setEnabled(t);
    ui->b_scenes_2->setEnabled(t);
    ui->b_videos_2->setEnabled(t);
    ui->b_scenes->setEnabled(t);
    ui->b_videos->setEnabled(t);
    ui->b_items->setEnabled(t);
    ui->b_items->setVisible(t);
    ui->b_scenes->setVisible(t);
    ui->b_videos->setVisible(t);
    ui->b_items_2->setVisible(t);
    ui->b_scenes_2->setVisible(t);
    ui->b_videos_2->setVisible(t);
}
void favItems::updateUi(){
    if( context  == "videos"){
        ui->lab_info_res->setEnabled(true);
    }
    else ui->lab_info_res->setEnabled(false);

    if(context.contains("fav_")){
        toggleFavButtons(true);
    } else toggleFavButtons(false);
    ui->lab_info_items->setEnabled(context == "fav_items");
    ui->lab_info_scenes->setEnabled( context == "fav_scenes");
    ui->lab_info_videos->setEnabled(context  == "fav_videos");

    ui->b_scenes->setStyleSheet( states[context  != "fav_scenes"] );
    ui->b_scenes_2->setStyleSheet( states[context  != "fav_scenes"] );
    ui->b_scenes->setIcon( wgen->star[context  != "fav_scenes"] );
    ui->b_scenes_2->setIcon( wgen->star[context  != "fav_scenes"] );
    ui->b_items->setStyleSheet( states[context  != "fav_items"] );
    ui->b_items_2->setStyleSheet( states[context  != "fav_items"] );
    ui->b_items->setIcon( wgen->star[context  != "fav_items"] );
    ui->b_items_2->setIcon( wgen->star[context  != "fav_items"] );
    ui->b_videos->setStyleSheet( states[context  != "fav_videos"] );
    ui->b_videos_2->setStyleSheet( states[context  != "fav_videos"] );
    ui->b_videos->setIcon( wgen->star[context  != "fav_videos"] );
    ui->b_videos_2->setIcon( wgen->star[context  != "fav_videos"] );
    ui->favs_cnt->update();
    repaint();
    wgen->forceUpdate();
}
void favItems::set_favs_cnt( int i ){
    ui->favs_cnt->setText(QString::number(i));
    wgen->forceUpdate();
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
