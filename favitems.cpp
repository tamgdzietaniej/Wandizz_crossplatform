#include "favitems.h"
#include "ui_favitems.h"
favItems::favItems( widgetGen* wg, QWidget *parent ) :
    QMainWindow( parent ),
    debg(false),
    wgen(wg),
    context(""),
    ui(new Ui::favItems)
{
    ui->setupUi( this );
    setAttribute( Qt::WA_DeleteOnClose, true );
    setAttribute( Qt::WA_TransparentForMouseEvents, false);
    connect(wg,SIGNAL(fav_update(int)),this,SLOT(set_favs_cnt(int)));
    connect(wg,SIGNAL(setLocked(bool)),this,SLOT(setLocked(bool)));
    star[1].addPixmap(QPixmap( ":/gui/icons/green_dot.png"));
    star[0].addFile(":/gui/icons/green_dot_no_dot.png");
    connect(wgen,SIGNAL(set_cnt(int)),this,SLOT(set_favs_cnt(int)));
    wgen->setParent(this);
    wgen->showFullScreen();
}
void favItems::setLocked(bool l){
    if(isEnabled()==l)setEnabled(!l);
    update();
}
bool favItems::setContext(QString c){
    qDebug()<<"SETCONTEXT:"<<c<<search_active;
    context=c;
    setObjectName(c);
    wgen->setContext(c);      /* NEW CONTEXT */
    updateUi();
    //  update();
    return true;
}
void favItems::activateContainer(){
    if(!wgen->isVisible()){
        wgen->showFullScreen();
        wgen->activateWindow();
        wgen->update();
    }
    if(!wgen->top_menu_switcher->isVisible()){
        wgen->top_menu_switcher->show();
    }
}
void favItems::init(){
    /* INIT once */
    opt_pos=mapToGlobal( ui->b_options->pos() );
    wgen->setDirector(this);
    //  wgen->setWindowFlag(Qt::FramelessWindowHint);
    wgen->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    wgen->showFullScreen();
    wgen->setGeometry(geo.a_geo);
    wgen->activateWindow();
    qDebug()<<"GLCONT GEO:"<<wgen->geometry();
    wgen->setFixedSize(geo.a_geo.size());
    qDebug()<<"SIZE:"<<wgen->size()<<size()<<geometry();
    //   update();
    /* SET HOT-SPOT HELPERS for switching contentd visible dependent of scroll */
    wgen->setMarkers();
    wgen->setMRB(0,ui->switch_b_frame_top->geometry().bottom(),0,10);
    wgen->setMRB(1,ui->switch_b_frame->geometry().bottom(),ui->switch_b_frame_top->geometry().bottom(),20);
    /* FILL GL WRAPPERS */
    wgen->setupTopMenuSwitcher(ui->switch_b_frame_top->geometry());
    wgen->setFrames(ui->switch_b_frame_top,ui->frame);
    wgen->setupWrapper({{},{ui->lab_favs_icon,ui->lab_info_items,ui->lab_info_res,ui->lab_info_scenes,ui->lab_info_videos,ui->favs_cnt,ui->prospect_info},{ui->switch_b_frame},{}});
    wgen->setProspectInfo(ui->prospect_info);
    wgen->updateGeometry();
}
void favItems::reinit( QString o, QStringList sr, bool cont ){
    qDebug()<<"Reinit:"<<search_active;
    wgen->reinit=true;
    qDebug()<<"FAVS:"<<sr;
    /* (RE)INIT VARS */
    search_active=sr.contains("search");
    if(!context.isEmpty()){
        wgen->restoreCoords();
    }
    setContext(o);
    wgen->setFrames(ui->switch_b_frame_top,ui->frame);
    if(!sr.contains("last"))
        wgen->top_menu_switcher->setSearchMode(search_active);
    params=sr;
    ui->b_back->setEnabled(cont);
    qDebug()<<"b:back:"<<cont;
    wgen->setSelectors(true);
    wgen->top_menu_switcher->raise();
    qDebug()<<"Reinit2:"<<search_active;
    setEnabled(true);
}
void favItems::hideEvent(QHideEvent *e){
    //   wgen->toggleBricks(false);
    wgen->top_menu_switcher->edit->releaseKeyboard();
}
/* FACTORY */

// prospect
void favItems::create_prospect_list(int* cnt3,QJsonArray *videos){
    int last_oid=-1,offs=0;
    for(int i=0;i<videos->count();i++){
        QJsonObject obj(videos->at(i).toObject());
        QString producer=obj.value("name").toString();
        if(obj.value("oid").toString().toInt()!=last_oid){
            QString logo_url=dataRoot+"pro_"+obj.value("oid").toString()+"/logo/logo.png";
            QString dname=d_logos+producer.replace( QRegExp( "[" + QRegExp::escape( "\\/:*?\"<>|" ) + "]" ),QString( "_" ) )+".png";
            wgen->make_brick(i+offs,
                             obj.value("oid").toString().toInt(),
                             0,
                             0,
                             obj.value("name").toString(),
                             obj.value("others").toString(),
                             logo_url,
                             dname,
                             "",
                             "prospect",
                             obj.value("oid").toString().toInt());
            offs++;
        }
        last_oid=obj.value("oid").toString().toInt();
        QStringList imData=wgen->getThumbRef(obj.value("filename").toString(), obj.value("name").toString());
        QString its;
        if(cnt3[obj.value("id").toString().toInt()]>0)
            its=QString::number( cnt3[obj.value("id").toString().toInt()] )+" items";
        wgen->make_brick(i+offs,
                         obj.value("id").toString().toInt(),
                         obj.value("id").toString().toInt(),
                         cnt3[ obj.value("id").toString().toInt() ],
                         obj.value("title").toString(),
                         its,
                         imData.takeFirst(),
                         imData.takeFirst(),
                         imData.takeFirst(),
                         "videos",
                         last_oid);
    }
    wgen->activate();
}

// fav videos
int favItems::getJInt(QJsonValue v){
    int cc=0;
    if(v.type()==QJsonValue::String)
        cc=v.toString().toInt();
    else cc=v.toInt();
    return cc;
}
void favItems::create_fvideos_list(int* cnt3,const QJsonArray* fav_titles){
    int cfm=fav_titles->count();
    for(int i=0;i<cfm;i++){
        QJsonObject obj=fav_titles->at(i).toObject();
        int fid=getJInt(obj.value("fav_id"));
        QStringList imData=wgen->getThumbRef(obj.value("filename").toString(),obj.value("title").toString());;
        QString its;
        if(cnt3[fid]>0)
            its=QString::number( cnt3[fid] )+" items";
        wgen->make_brick( i,
                         fid,
                         fid,
                         cnt3[fid],
                         obj.value("title").toString(),
                         its,
                         imData.takeFirst(),
                         imData.takeFirst(),
                         imData.takeFirst(),
                         "fav_videos",
                         getJInt(obj.value("oid")));
    }

    wgen->activate();
    set_favs_cnt(cfm);
}

// all videos
void favItems::create_videos_list(int* cnt3,const QJsonArray* videos){
    int cfm=videos->count();
    for(int i=0;i<cfm;i++){
        QJsonObject obj=videos->at(i).toObject();
        int fid=getJInt(obj.value("id"));
        QStringList imData=wgen->getThumbRef(obj.value("filename").toString(),  obj.value("title").toString());
        QString its;
        if(cnt3[fid]>0)
            its=QString::number( cnt3[fid] )+" items";
        wgen->make_brick(i,
                         fid,
                         fid,
                         cnt3[fid],
                         obj.value("title").toString(),
                         its,
                         imData.takeFirst(),
                         imData.takeFirst(),
                         imData.takeFirst(),
                         "videos",
                         getJInt(obj.value("oid")));
    }
    set_favs_cnt(cfm);
    wgen->activate();

}
// fav scenes
void favItems::create_scenes_list(int* cnt3,const QJsonArray* jfs){
    int cfs=jfs->count();
    for(int i=0;i<cfs;i++){
        QJsonObject obj=jfs->at(i).toObject();
        int fid=getJInt(obj.value("fav_id"));
        wgen->make_brick( i,
                         fid,
                         getJInt(obj.value("title_id")),
                         cnt3[getJInt(obj.value("title_id"))],
                         obj.value("title").toString(),
                         obj.value("time").toString(),
                         obj.value("scene_image").toString(),
                         d_carousel+QUrl(obj.value("scene_image").toString()).fileName(),
                         serverRoot+obj.value("filename").toString(),
                         "fav_scenes",
                         getJInt(obj.value("oid")),
                         obj.value("event").toString());
    }
    set_favs_cnt(cfs);
    wgen->activate();
}

// fav items
void favItems::create_items_list(const QJsonArray* jfi){
    int cfi=jfi->count();
    for(int i=0;i<cfi;i++){
        QJsonObject obj=jfi->at(i).toObject();
        qDebug()<<obj;
        int fid=getJInt(obj.value("fav_id"));
        wgen->make_brick( i,
                         fid,
                         getJInt(obj.value("title_id")),
                         0,
                         obj.value("name").toString()+"\n from",
                         obj.value("title").toString(),
                         obj.value("image").toString(),
                         d_carousel+QUrl( obj.value("image").toString() ).fileName(),
                         "","fav_items",
                         getJInt(obj.value("oid")),
                         obj.value("url").toString());
    }
    set_favs_cnt(cfi);
    wgen->activate();
}

// some style-scrap
void favItems::setStyles(QPushButton* w,QString f){
    w->setStyleSheet( states[context.contains(f)] );
    w->setIcon( star[context.contains(f)]);
    w->update();
}

// search mode toggler
void favItems::setMode(QWidget* w,QString f,bool s){
    if(f=="")f="fav";
    if(!s){
        s=!search_active;
    }
    w->setEnabled(context.left(f.length())==f && s);
    w->setVisible(context.left(f.length())==f && s);
    w->update();
}

// configure functionlity and access to module fuctions
void favItems::updateUi(){
    setMode(ui->lab_info_res,"videos");
    setMode(ui->lab_info_items,"fav_items");
    setMode(ui->lab_info_scenes,"fav_scenes");
    setMode(ui->lab_info_videos,"fav_videos");
    setMode(ui->b_items);
    setMode(ui->b_items_2,"",false);
    setMode(ui->b_scenes);
    setMode(ui->b_scenes_2,"",false);
    setMode(ui->b_videos);
    setMode(ui->b_videos_2,"",false);
    ui->lab_favs_icon->setEnabled(context!="prospect");
    ui->lab_favs_icon->setVisible(context!="prospect");
    ui->favs_cnt->setEnabled(context!="prospect");
    ui->favs_cnt->setVisible(context!="prospect");
  //  ui->b_items->setEnabled(!context.contains("items"));
 //   ui->b_scenes->setEnabled(!context.contains("scenes"));
 //   ui->b_videos->setEnabled(!context.contains("videos"));
 //   ui->b_items_2->setEnabled(!context.contains("items"));
  //  ui->b_scenes_2->setEnabled(!context.contains("scenes"));
  //  ui->b_videos_2->setEnabled(!context.contains("videos"));
    setStyles(ui->b_scenes,"fav_scenes");
    setStyles(ui->b_scenes_2,"fav_scenes");
    setStyles(ui->b_items,"fav_items");
    setStyles( ui->b_items_2,"fav_items");
    setStyles(ui->b_videos,"fav_videos");
    setStyles(ui->b_videos_2,"fav_videos");
    update();
}

// items counter - SET
void favItems::set_favs_cnt( int i ){
    ui->favs_cnt->setText(QString::number(i));
    update();
}


/* UI BUTTONS MANAGEMENT*/
void favItems::on_b_items_clicked(){
    if( context  == "fav_items")return;
    emit go( "fav_items",params );
}
void favItems::on_b_scenes_clicked(){
    if(  context  == "fav_scenes")return;
    emit go( "fav_scenes",params);
}
void favItems::on_b_videos_clicked(){
    if( context  == "fav_videos")return;
    emit go( "fav_videos",params );
}
void favItems::on_b_back_clicked(){
    emit go( "back",{} );
}
void favItems::on_b_options_clicked(){
    qDebug()<<"OPT@1"<<  mapToGlobal( ui->b_options_2->pos() )<<ui->b_options_2->pos();
    emit go("menu",{} );
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

// LAST LINE

void favItems::on_b_full_wandizz_clicked()
{
    setEnabled(false);
    emit go("full",{});
}
