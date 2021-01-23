#include "metasearch.h"
#include "ui_metasearch.h"
metasearch::metasearch( widgetGen* wg, QWidget *parent ) :
    QMainWindow( parent ),debg(false),wgen(wg), ui( new Ui::metasearch )
{
    ui->setupUi( this );
    //   setAttribute( Qt::WA_AlwaysStackOnTop, true );
    //   setAttribute( Qt::WA_NoSystemBackground, true);
    //  setAttribute( Qt::WA_TranslucentBackground, true );
    setAttribute( Qt::WA_DeleteOnClose, true );
    setAttribute( Qt::WA_TransparentForMouseEvents, false );
    setObjectName("prospect");
    input=QApplication::inputMethod();
    corr_y=0;

}
bool metasearch::setContext(QString c){
    qDebug()<<"GLL:"<<wgen->glcontainer->geometry()<<wgen->mrh()<<wgen->top_menu_switcher->geometry();
    wgen->setContext(c);
    /* NEW CONTEXT */
    wgen->setDirector(this);
    wgen->setMFCorr( 0, vcorr.bottom()+30);
    wgen->glcontainer->setupWrapper({{},{},{}});
    show();
    wgen->glcontainer->show();
    wgen->glcontainer->update();
    wgen->start_bg_proc();
    qDebug()<<"GLL:"<<wgen->glcontainer->geometry()<<wgen->mrh()<<wgen->top_menu_switcher->geometry();;
    //     ui->searchFrame->setStyleSheet("border:1px solid #00000050;background:qlineargradient(spread:reflect, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(70,40,40,130), stop:1 rgba(70,40,40,130));border-radius:10px;color:#9f9f9f;");

    update();
}

void metasearch::init(){
    wgen->marker_last.move(rect().bottomLeft());
    vcorr=ui->switch_b_frame_top->geometry();
}

void metasearch::reinit( bool cont ){
    wgen->setupTopMenuSwitcher(ui->switch_b_frame_top->geometry());
    wgen->setFrames(ui->switch_b_frame_top,ui->frame);
    QString o="prospect";
    setObjectName(o);
    if(!setContext(o)){
        qDebug()<<"New context:"<<wgen->getContext();
    }
    wgen->setSelectors(true);
    ui->b_back->setEnabled(cont);
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

void metasearch::create_prospect_list(int* cnt3,QJsonArray *videos){
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
    wgen->activate(true);
}
void metasearch::on_b_back_clicked(){
    //wgen->tgs=2;
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
metasearch::~metasearch(){
    while(!wgen->poster_data.isEmpty() && !wgen->poster_data_ready.isEmpty()){
        QApplication::processEvents();
        qDebug()<<"Queue:"<<wgen->poster_data.count()<<"/"<<wgen->poster_data_ready.count();
    }
    qDebug()<<"DELETING Prosp";
    delete ui;
}
/*
void metasearch::on_b_sh_night_clicked(bool checked){
    wgen->glcontainer->nightmode=checked;
    wgen->glcontainer->forceUpdate();
}
*/
