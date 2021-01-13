#include "metasearch.h"
#include "ui_metasearch.h"
metasearch::metasearch( widgetGen* wg, QWidget *parent ) :
    QMainWindow( parent ),debg(false),wgen(wg), ui( new Ui::metasearch )
{
    ui->setupUi( this );
    setAttribute( Qt::WA_AlwaysStackOnTop, true );
    setAttribute( Qt::WA_NoSystemBackground, true);
    setAttribute( Qt::WA_TranslucentBackground, true );
    setAttribute( Qt::WA_DeleteOnClose, true );
    setAttribute( Qt::WA_TransparentForMouseEvents, false );
    ui->m_filters->setWindowFlag(Qt::FramelessWindowHint);
    ui->m_filters->setAttribute(Qt::WA_AlwaysStackOnTop);
    setObjectName("prospect");
}
bool metasearch::setContext(QString c){
    wgen->lock();
    if(wgen->setContext(c)){
        /* NEW CONTEXT */
        wgen->setDirector(this);
        wgen->set_markers(&marker_first,&marker_middle,&marker_last,&marker,&marker_switch);
        wgen->setupWrapper({{ui->logo},{},{ui->switch_b_frame},{}});
        if(wgen->par->search_switch_with_top_bar)
            wgen->setFrames({ui->switch_b_frame},{ui->frame});
        else
            wgen->setFrames({ui->switch_b_frame_top},{ui->frame});
    }
    wgen->hideRastered(ui->centralwidget);
}
void metasearch::init(){
    wgen->setupTopMenuSwitcher(ui->switch_b_frame_top->geometry());
    setMarkers();
}

void metasearch::reinit( bool cont ){
    QString o="prospect";
    if(!setContext(o)){
        qDebug()<<"New context:"<<wgen->getContext();
    }
    wgen->showSelectors();
    ui->b_back->setEnabled(cont);
}
void metasearch::hideEvent( QHideEvent*e){
    e->accept();
    wgen->lock();
}
void metasearch::mouseMoveEvent( QMouseEvent*e ){
    wgen->mouseMove(e);
}
void metasearch::mousePressEvent( QMouseEvent *e ){
    wgen->mousePress(e);
}
void metasearch::mouseReleaseEvent( QMouseEvent *e ){
    if(wgen->mouseRelease(e)){
        if(wgen->currType()=="prospect"){
            emit go("userprof",{"context",wgen->currItem()->txt_text,QString::number(wgen->currItem()->db_index)});
        }
    }
    wgen->over_brick=-1;
}

void metasearch::create_prospect_list(int* cnt3,QJsonArray *videos,QJsonArray *f_titles){
    int last_oid=-1,offs=0;
    wgen->set_fav_videos_list(f_titles);
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
        if(cnt3[obj.value("id").toString().toInt()]>0)its=QString::number( cnt3[obj.value("id").toString().toInt()] )+" items";
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
    wgen->activate(true);
}
void metasearch::setMarkers(){
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
void metasearch::on_b_back_clicked(){
    emit go( "back",{} );
}
void metasearch::on_b_back_2_clicked(){
    ui->b_back->click();
}
void metasearch::on_b_options_clicked(){
    emit show_menu( ui->b_options->mapToGlobal( QPoint( 0, 0 ) ) );
}
void metasearch::on_b_options_2_clicked(){
    qDebug()<<"b_opt2 ms";
    ui->b_options->click();
}
void metasearch::on_b_home_clicked(){
    ui->searchEdit->clear();
}

void metasearch::on_b_filters_clicked(){
    qDebug()<<"FILTER:"<<ui->m_filters->isVisible();
    ui->m_filters->setVisible(!ui->m_filters->isVisible());
    qDebug()<<"FILTER2:"<<ui->m_filters->isVisible();
    wgen->glcontainer->update();
}
void metasearch::on_b_sh_titles_stateChanged(int st){
    wgen->par->prospect_sh_items=st;
    wgen->toggleBricks();
    qDebug()<<"TOGGLE:"<<st;
}
void metasearch::on_b_sh_titles_clicked()
{
    qDebug()<<"click:";
}
metasearch::~metasearch(){
    while(wgen->poster_data.isEmpty() && wgen->poster_data_ready.isEmpty()){
        QApplication::processEvents();
        qDebug()<<"Queue:"<<wgen->poster_data.count()<<"/"<<wgen->poster_data_ready.count();
    }
    if(future.isRunning())future.waitForFinished();
    qDebug()<<"DELETING Prosp";
    delete ui;
}


