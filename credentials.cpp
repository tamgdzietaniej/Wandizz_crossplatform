#include "credentials.h"
Credentials::Credentials( QObject *parent ): QObject( parent )  ,
    lat( "" )  ,
    lon( "" )  ,
    offline_m( false )  ,
    recovery( false )  ,
    hsh( "" )  ,
    api_ind( 1 )  , recov( false ), social_type(0), debg(false)
{
    make_dir( hash_dir );
    make_dir( d_carousel );
    make_dir( d_recorder );
    make_dir( d_posters );
    sql_api.setParent(0);
}
void Credentials::make_dir( QString dir ){
    QDir d;
    d.setPath( dir );
    if( !d.exists( dir ) ){
        if( !d.mkdir( dir ) ){
            if(debg)qDebug()<< dir<< " NOT CREATED";
            informat( "CANT ACCESS STORAGE DEVICE" );
            QApplication::quit();
        }
    }
}
void Credentials::informat( QString inf ){
    emit inform( inf );
}
void Credentials::process(){
    manager = new QNetworkAccessManager(this);
    connect( manager  , SIGNAL( finished( QNetworkReply* ) )  , this  , SLOT( collect_replies( QNetworkReply* ) ), Qt::QueuedConnection );
    upmanager = new QNetworkAccessManager(this);
    connect( upmanager  , SIGNAL( finished( QNetworkReply* ) )  , this  , SLOT( upload_finished( QNetworkReply* ) ));
    imgs_cnt = 0;
    curr_id = 0;
    s_curr_id = "";
    curr_user = "";
    curr_nick = "";
    curr_phone = "";
    jfi = QJsonDocument();
    jfs = QJsonDocument();
    jfm = QJsonDocument();
    get_titles();
    get_remote_params();
    emit go_get_user_data();
}
int Credentials::make_request( QString ident ){
    QNetworkRequest* request=new QNetworkRequest;
    request->setAttribute( QNetworkRequest::User  , api_ind );
    request->setUrl( api_url+ident );
    manager->get( *request );
    if(debg)qDebug()<<"REQ:"<<request->url();
    return api_ind++;
}
int Credentials::pull_query( QUrl url ){

    QNetworkRequest* request=new QNetworkRequest;
    request->setAttribute( QNetworkRequest::User  , api_ind );
    request->setRawHeader( "Keep-Alive"  , "timeout = 5  , max = 200;" );
    request->setRawHeader( "Cache-Control"  , "no-cache" );
    request->setRawHeader( "Cache-Control"  , "no-store" );
    request->setAttribute( QNetworkRequest::HttpPipeliningAllowedAttribute  , true );
    request->setUrl( url );
    manager->get( *request );
    qDebug()<<"CRED:PULL QUERY:"<<request->url();
    return api_ind++;
}
void Credentials::collect_replies( QNetworkReply* reply ){
    replies.append( reply );
}
QList< QByteArray> Credentials::get_reply( int id ){
    QList< QByteArray> reply;
    reply.clear();
    int rc = replies.count();
    for( int i = 0;i< rc;i++ ){
        if( replies.at( i )->request().attribute( QNetworkRequest::User ).toInt() == id ){
            QByteArray rep=replies.at(i)->readAll();
            reply.append( rep );
            // if(debg)qDebug()<<"CRED:GOT REPLY:"<<rep;
            replies.takeAt(i)->deleteLater();
            break;
        }
    }
    return reply;
}
QList< QByteArray> Credentials::wait_and_get_reply( int id ){
    QList< QByteArray> reply;
    while( reply.empty() ){
        reply = get_reply( id );
        QApplication::processEvents();
    }
    return reply;
}
/*
 * USER AUTHORIZATON USER AUTHORIZATON USER AUTHORIZATON USER AUTHORIZATON USER AUTHORIZATON USER AUTHORIZATON USER AUTHORIZATON USER AUTHORIZATON *
 */

void Credentials::get_user_data(){
    if( !check_signature() ){
        if(debg)qDebug()<< offline_m;
        if( !offline_m ){
            if(debg)qDebug()<< "Adding user";
            add_new_user();
        }
    }
    emit got_user_data();
}
void Credentials::get_user_by_email( QString email ){
    sql_api.set_method( sqlApi::SELECT);
    sql_api.add_table("users");
    sql_api.add_field("*");
    sql_api.add_equ("name",email);
    QJsonDocument d = get_json_result();
    if(set_user(d)){
        emit got_user_data();
    }
    else {
        qDebug()<<"WRONG EMAIL:"<<email;
    }
}
QJsonDocument Credentials::get_json_result(){
    return is_data( wait_and_get_reply( pull_query( sql_api.get_query() ) ) );
}
bool Credentials::update_user_activity( QString activity ){
    sql_api.set_method( sqlApi::INSERT );
    sql_api.add_table( "user_activity" );
    sql_api.add_default();
    sql_api.add_insdata( curr_id );
    sql_api.add_insDate();
    sql_api.add_insdata( activity );
    sql_api.add_insdata( lat );
    sql_api.add_insdata( lon );
    pull_query( sql_api.get_query() );
    return true;
}
int Credentials::geti( QJsonDocument doc  , QString key  , int i ){
    if( doc.isNull() )return 0;
    QJsonValue val;
    if( doc.isArray() ){
        val = doc.array().at( i );
        val = val.toObject().take( key );
    } else {
        val = doc.object().take( key );
    }
    if( val.isString() )return val.toString().toInt();
    else return val.toInt();
}
QString Credentials::gets( QJsonDocument doc  , QString key  , int i ){
    if( doc.isNull() )return "";
    QJsonValue val;
    if( doc.isArray() ){
        val = doc.array().at( i );
        val = val.toObject().take( key );
    } else {
        val = doc.object().take( key );
    }
    return val.toString();
}
void Credentials::get_user_data( QString login  , QString pass, int social){
    //  if(debg)qDebug()<< "GET USER DATA:"<< login<< pass;
    recovery = true;
    social_type=social;
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "users" );
    sql_api.add_field( "*" );
    sql_api.add_equ( "name"  , login );
    if(social==0)sql_api.add_pass_equ( "pass"  , pass );
    if(debg)qDebug()<< "REC USERQUERY:"<< sql_api.get_query();
    QJsonDocument r = is_data( wait_and_get_reply( pull_query( sql_api.get_query() ) ) );
    //    if(debg)qDebug()<< "R:"<< r;
    //   if(debg)qDebug()<< "RE:"<< r.array().isEmpty()<< r.array().count();
    if(!set_user(r))
        emit rec_acc_not_found();
}
bool Credentials::set_user(QJsonDocument r){
    if( !r.array().isEmpty() ){
        curr_id = geti( r  , "id" );
        curr_user = gets( r  , "name" );
        curr_pass = gets( r  , "pass" );
        curr_join = gets( r  , "joined" );
        curr_nick = gets( r  , "nick" );
        curr_phone = gets( r  , "phone" );
        hsh = gets( r  , "hwid" );
        update_user_activity( "Account recovery for "+curr_user );
        QString avatar_file = curr_user.toLower();
        avatar_file = avatar_file.replace( "@"  , "" );
        avatar_file = avatar_file.replace( "."  , "" );
        if(debg)qDebug()<< "Recover user avatar:"<< f_user_avatar<< "\n"<< uploaded_url+avatar_file.toLatin1()+".png";
        QFile file(f_user_avatar);
        file.open(QFile::WriteOnly);
        if(debg)qDebug()<<"DOWNLOADER";
        file.write( downloaderStd().download( uploaded_url+avatar_file.toLatin1()+".png" ) );
        if(debg)qDebug()<<"DOWNLOADER";
        file.close();
        QPixmap newP(f_user_avatar);
        QPixmap newP2(circlePix(newP));
        newP2.save(f_user_avatar);

        if(debg)qDebug()<< "NEW DATA:"<< hsh<< curr_id;
        write_hsh();

        //   emit close_profsettings();
        recov = true;
        if(debg)qDebug()<<"GOT USER DATA";
        emit got_user_data();
        return true;
    } else return false;
}

QByteArray Credentials::get_jwt(){
    QByteArray r = is_raw_data( wait_and_get_reply( pull_query( serverRoot+"/api/jwt.php") ) );
    qDebug()<<"JWT SET:"<<r;
    return r;
}
void Credentials::get_remote_params(){
    //  if(debg)qDebug()<< "GET USER DATA:"<< login<< pass;
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "appsettings" );
    sql_api.add_field( "var,val" );
    sql_api.add_equ( "user"  , "all_users");
    qDebug()<< "PARAMS REQUEST:"<< sql_api.get_query();
    QJsonDocument r = is_data( wait_and_get_reply( pull_query( sql_api.get_query() ) ) );
    //    if(debg)qDebug()<< "R:"<< r;
    //   if(debg)qDebug()<< "RE:"<< r.array().isEmpty()<< r.array().count();
    qDebug()<<"PARAMS SET:"<<r.isArray();
    if( !r.array().isEmpty() ){
        QJsonArray ar=r.array();
        qDebug()<<"Params count:"<<ar.count();
        for(int i=0;i<ar.count();i++){
            QJsonObject json(ar.at(i).toObject());
            qDebug()<<json.value("var")<<"="<<json.value("val");
            QString val=json.value("val").toString();
            QString var=json.value("var").toString();
            if(var=="carousel_video_duration")player_offset=val.toInt();
            if(var=="carousel_video")player_enabled=val.toInt();
            if(var=="carausel_sliders_mutithreading")
            qDebug()<<player_offset<<player_enabled;
        }
    }
}
QPixmap Credentials::circlePix(QPixmap p){
    int w=p.width();
    int h=p.height();
    if(w>h){
        // rot=false;
        w=h;
    } else {
        //  rot=true;
        h=w;
    }
    int x=p.width()/2-w/2;
    int y=p.height()/2-h/2;
    QPixmap p2(w,h);
    QPainter painter(&p2);
    if(debg)qDebug()<<"photoo dimms:"<<p.width()<<p.height();
    painter.drawPixmap(0,0,w,h,p,x,y,w,h);
    painter.end();
    QBitmap map(w,h);
    map.fill(Qt::color0);
    painter.begin(&map);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setBrush(QColor(0,0,0,255));
    QPen pen;
    pen.setWidth(3);
    painter.drawEllipse(0,0,w,h);
    p2.setMask(map);
    painter.end();
    painter.begin(&p2);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setPen(pen);
    painter.drawEllipse(0,0,w,h);
    painter.end();
    return p2;
}
bool Credentials::update_loc(){
    sql_api.set_method( sqlApi::INSERT );
    sql_api.add_table( "trip" );
    sql_api.add_insdata( curr_id );
    sql_api.add_insdata( lat );
    sql_api.add_insdata( lon );
    sql_api.add_insDate();
    sql_api.send_query();
    return true;
}
bool Credentials::check_signature(){
    hsh = "";
    curr_id = 0;
    if( QFileInfo::exists( f_app_hash ) ){
        if(debg)qDebug()<< "Hash exists";
        hsh = getFile( f_app_hash );
        sql_api.set_method( sqlApi::SELECT );
        sql_api.add_table( "users" );
        sql_api.add_field( "*" );
        sql_api.add_equ( "hwid"  , hsh );
        QUrl query = sql_api.get_query();
        int qid = pull_query( query );
        QJsonDocument r = is_data( wait_and_get_reply( qid ) );
        if(debg)qDebug()<<"check hash query:"<<query;
        if(debg)qDebug()<<"check hash in db-result:"<<r;

        if( !r.isEmpty() ){
            curr_id = geti( r  , "id" );
            curr_user = gets( r  , "name" );
            curr_pass = gets( r  , "pass" );
            curr_join = gets( r  , "joined" );
            curr_nick = gets( r  , "nick" );
            curr_phone = gets( r  , "phone" );
        }
    }
    if(debg)qDebug()<< "CURR:"<< curr_id<< hsh;
    return curr_id != 0;

}
bool Credentials::add_new_user(){

    QString n_ext = QTime::currentTime().toString( "HHmmsszzz" );
    QString guest = "GUEST_"+n_ext;
    curr_user = guest;
    return add_new_user( guest  , n_ext );
}
bool Credentials::add_new_user( QString login, QString pass, QString nick, QString token, int social ){
    //user_activity( "Remove hsh" );
    if( QFileInfo::exists( f_app_hash ) )QFile::remove( f_app_hash );
    QString hsh = get_hsh( pass  , true );
    sql_api.set_method( sqlApi::INSERT );
    sql_api.add_table( "users" );
    sql_api.add_default();
    sql_api.add_insdata( login );
    sql_api.add_insdata( hsh );
    sql_api.add_insDate();
    sql_api.add_insdata( nick );
    sql_api.add_insdata( "" );
    sql_api.add_insdata( social==1 );
    sql_api.add_insdata( social==2 );
    sql_api.add_insdata( lat );
    sql_api.add_insdata( lon );
    sql_api.add_insdata( pass  , true );
    sql_api.add_insdata( 0 );
    sql_api.add_insdata( token );
    sql_api.add_insdata(0);
    sql_api.add_insdata(0);
    QJsonDocument r = is_data( wait_and_get_reply( pull_query( sql_api.get_query() ) ) );
    if( !r.isEmpty() ){
        curr_id = geti( r  , "lastinsertid" );
        writeFile( f_user_login  , login.toLatin1() );
        writeFile( f_user_nick  , nick.toLatin1() );
        writeFile( f_user_id  , QString::number( curr_id ).toLatin1() );
        if(debg)qDebug()<< "ADDED:"<< curr_id;
        update_user_activity( "New user added" );
        return true;
    } else return false;
}
QJsonDocument Credentials::is_data( QList< QByteArray> r ){
    QJsonDocument r1( QJsonDocument::fromJson( r.takeFirst() ) );
    if( r1.isArray() ){
        //    if(debg)qDebug()<< "Response************** array "<< r1.array();
        if( r1.array().isEmpty() ){
            if(debg)qDebug()<< " ARRAY EMPTY";
            return  QJsonDocument();
        }
    }
    if( r1.isObject() ){
        //     if(debg)qDebug()<< "Response************** object "<< r1.array();
        if( r1.object().isEmpty() ){
            if(debg)qDebug()<< " OBJECT EMPTY";
            return QJsonDocument();
        }
    }
    return r1;

}
QByteArray Credentials::is_raw_data( QList< QByteArray> r ){
    return r.takeFirst();
}
int Credentials::check_login( QString login ){
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "users" );
    sql_api.add_field( "*" );
    sql_api.add_equ( "name"  , login );
    QJsonDocument r = is_data( wait_and_get_reply( pull_query( sql_api.get_query() ) ) );
    if( !r.isEmpty() ){
        return geti( r  , "id" );
    }
    return 0;
}
int Credentials::check_user( QString login ){
    int chk_id = check_login( login );
    if( chk_id != 0 && chk_id != curr_id ){
        return chk_id;
    } else {
        return 0;
    }
}
void Credentials::update_user( QString login  , QString pass  , QString nick  , QString phone ){
    if(debg)qDebug()<< "CURR ID:"<< curr_id;
    sql_api.set_method( sqlApi::UPDATE );
    sql_api.add_table( "users" );
    sql_api.add_updata( "name"  , login );
    sql_api.add_updata( "nick"  , nick );
    sql_api.add_updata( "phone"  , phone );
    if( pass != "" ){
        sql_api.add_updata( "pass"  , pass  , true );
    }
    sql_api.add_cond( "id = "+QString::number( curr_id ) );
    sql_api.send_query( true );
    curr_user = login;
    upload_file( f_user_avatar );
}

//bool Credentials::check_update(){
//    return false;
//#if defined (Q_OS_ANDROID)
//    downloaderStd downloader;
//    QString updates = downloader.download( chk_update_url );
//    updates = updates.replace( "\r"  , "" );
//    updates = updates.replace( "\n"  , "" );
//    int ver = updates.toInt();
//    QString curr_version_s=updates;
//    if(debg)qDebug()<< "WAND:CREDENTIALS:-- current version:"<< version<< "  , "<< updates;
//    if( ver != version ){
//        QString uurl=dwn_update_url+curr_version_s+"/android-build-debug.apk";

//        if(debg)qDebug()<< "WAND:CREDENTIALS:-- updating to version "<< updates << " from "<< uurl;
//        // update_user_activity( "APP UPDATE NEEDED" );
//        if(debg)qDebug()<<"REQUEST URL:"<<uurl;
//        //      connect(&downloader,SIGNAL(progress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
//        QByteArray apkDataDytes=downloader.download(uurl);
//        if(debg)qDebug()<<"UPGRADER DATA SIZE: "<<apkDataDytes.size()<<" BYTES.";
//        QString dest_file=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +"/android-build-release-signed.apk";
//        //     if(QFile::exists(dest_file)){
//        //          QFile::remove(dest_file);
//        //          if(QFile::exists(dest_file)){
//        //              if(debg)qDebug()<<"PLIK DALEJ ISTNIEJE!!!!!!!!!!!!!!!!";
//        //          } else {
//        //              if(debg)qDebug() << "JUZ NIE ISTNIEJE";
//        //          }
//        //      }
//        if(debg)qDebug()<<"PROCESSING...";
//        QFile* f=new QFile();
//        f->setFileName(dest_file);
//        f->open(QIODevice::WriteOnly);
//        f->write(apkDataDytes);
//        f->close();
//        //      if(debg)qDebug()<<"WAITING FOR FILE WRITE";
//        //      while(f->isOpen())
//        //     {}
//        if(QFile::exists(dest_file)){
//            QString jfile=dest_file.replace("file:","content:");
//            QAndroidJniObject jsText = QAndroidJniObject::fromString(jfile);
//            if(debg)qDebug()<<"FILE DOWNLOADED OK ("<<jfile<<")";

//            QAndroidJniObject::callStaticMethod<jint>("upgrader/upgrader/upgrader",
//                                                      "installApp",
//                                                      "(Ljava/lang/String;)I",
//                                                      jsText.object<jstring>());
//        } else {
//            if(debg)qDebug()<<"UPDATE FAILED-FILE DOES NOT EXIST";
//        }
//        return true;
//    } else
//#endif
//        return false;
//}
void Credentials::downloadProgress(qint64 p,qint64 tot){
    int perc=(qreal(tot-p)/qreal(tot))*100;
    if(debg)qDebug()<<"DOWNLOADPROGRESS:"<<p<<tot<<perc<<" left";
}
bool Credentials::upload_file( QString filename ){
    return false;
    qDebug()<<"Credentials::upload_file("<<filename<<")";
    QFile *upfile=new QFile(filename);
    while(!QFileInfo(*upfile).isReadable()){
        qDebug()<<"!!!readable";
        QApplication::processEvents();
    }
    QString avatar_file=curr_user.toLower();
    avatar_file=avatar_file.replace("@","");
    avatar_file=avatar_file.replace(".","");
    QByteArray avatar_name=avatar_file.toLatin1()+".png";
    QUrl url(ftp_avatars+"/"+avatar_name);
    url.setUserName("appavatar");    // Set login
    url.setPassword("Wandizz213!"); // Set пароль
    url.setPort(21);             // Protocol port, which we will work on
    qDebug()<<"upfile:"<<upfile;
    upfile->open(QIODevice::ReadOnly);

    // Start upload
    upmanager->put(QNetworkRequest(url), upfile);




    //    manager->moveToThread(thread());

    //    connect( upmanager  , SIGNAL( finished( QNetworkReply* ) )  , this  , SLOT( upload_finished( QNetworkReply* ) ));
    //    upfile->open(QIODevice::ReadOnly);
    //    QString bound="margin"; //name of the boundary
    //   QNetworkRequest* request=new QNetworkRequest;

    //   QString avatar_file=curr_user.toLower();
    //   avatar_file=avatar_file.replace("@","");
    //   avatar_file=avatar_file.replace(".","");
    //   QByteArray avatar_name=avatar_file.toLatin1()+".png";
    //       QByteArray data(QString("--" + bound + "\r\n").toLatin1());
    //       data.append("Content-Disposition: form-data; name='action'\r\n\r\n");
    //     data.append("avatar_upload.php\r\n"); //our script's name, as I understood. Please, correct me if I'm wrong
    //      data.append("--" + bound + "\r\n"); //according to rfc 1867
    //      data.append("Content-Disposition: form-data; name='uploaded'; filename='"+avatar_name+"'\r\n"); //name of the input is "uploaded" in my form, next one is a file name.
    //     data.append("Content-Type: image/png\r\n\r\n"); //data type
    //     data.append(upfile->readAll()); //let's read the file
    //       data.append("\r\n");
    //      data.append("--" + bound + "--\r\n"); //closing boundary according to rfc 1867
    //    request->setUrl(QUrl("https://producer.wandizz.com/index.php"));
    //     request.setRawHeader(QString("Content-Type").toLatin1(),QString("multipart/form-data; boundary=" + bound).toLatin1());
    //     request.setRawHeader(QString("Content-Length").toLatin1(), QString::number(data.length()).toLatin1());
    //    qDebug()<<"-- file opened:"<<data;
    //  qDebug()<<"REQ:"<<request->url();
    // upmanager->get(*request);
}

void Credentials::upload_finished( QNetworkReply* reply ){
    QString errstr = reply->errorString();
    qDebug()<<"REPLY:"<<reply->readAll();
    qDebug()<<"ERR:"<<errstr;
    update_user_activity( "New avatar picture update "+errstr );
}
QString Credentials::get_hsh( QString str  , bool store ){
    QByteArray pass_br( str.toStdString().c_str() );
    hsh = QCryptographicHash::hash( pass_br  , QCryptographicHash::Sha256 ).toHex();
    //  hsh = QCryptographicHash::hash( "wandizz"  , QCryptographicHash::Sha256 ).toHex();
    if( store ){
        write_hsh();
    }
    return hsh;
}
bool Credentials::write_hsh(){
    // update_user_ctivity( "Credentials::write_hsh()" );
    if( !writeFile( f_app_hash  , hsh.toLatin1() ) ){
        inform( "CANNOT SAVE HASHFILE TO: "+f_app_hash );
        if(debg)qDebug()<< "WAND:CREDENTIALS:CANT WRITE FILE!";
        //  qApp->quit();
        return false;
    } else {
        if(debg)qDebug()<< "WAND:CREDENTIALS:WROTE HASH:"<< f_app_hash<< hsh.toLatin1();
        QByteArray chkhsh = readFile( f_app_hash );
        if(debg)qDebug()<< "Checkhsh:"<< chkhsh;
        return true;
    }
}
QString Credentials::randHash( int len ){
    qsrand( time( nullptr ) );
    for( int i = 0; i< len; ++i ){
        int index = qrand()% possibleCharacters.length();
        QChar nextChar = possibleCharacters.at( index );
        randomString.append( nextChar );
    }
    return randomString;
}
/*
 * FAVOURITES FAVOURITES FAVOURITES FAVOURITES FAVOURITES FAVOURITES FAVOURITES FAVOURITES FAVOURITES FAVOURITES FAVOURITES FAVOURITES *
 */
void Credentials::get_titles(){
    if(debg)qDebug()<<"GET TILES";
    int resid = make_request( "getTitles" );
    QTimer* timer = new QTimer( );
    timer->setInterval( 100 );
    int titcnt = 0;
    bool replied = false;
    timer->start( 0 );
 //   connect( this  , SIGNAL( got_titles( QJsonArray ) )  , timer  , SLOT( deleteLater() ) );
    connect( this  , SIGNAL( got_titles( QJsonArray ) )  , timer  , SLOT( stop() ) );
    connect( timer  , &QTimer::timeout  , [ = , &titcnt  , &replied](){
        QList< QByteArray> reply = get_reply( resid );
        if( !reply.empty() ){
            replied = true;
            QByteArray res;
            res = reply.takeFirst();
            res.replace( "\\"  , "" );
            if(debg)qDebug()<<"TITLES JSON:"<<reply<<"???????"<<res<<"???????"<<QJsonDocument::fromJson(res);
            titles = new QJsonArray(QJsonDocument::fromJson( res ).array());
            titcnt = titles->count();
            //    sql_api.selectQuery({"timeline"},{"title_id","COUNT( * )AS count"},{"title_id"});
            sql_api.clear_all();
            sql_api.set_method( sqlApi::SELECT );
            sql_api.add_table( "timeline" );
            sql_api.add_field( "title_id" );
            sql_api.add_field( "COUNT( * )AS count" );
            sql_api.set_groupby( "title_id" );
            QList< QByteArray> reply = wait_and_get_reply( pull_query( sql_api.get_query() ) );
            if( !reply.empty() ){
                gt = QJsonDocument::fromJson(reply.takeFirst());
                QByteArray gtb = gt.toBinaryData();
                if( !writeFile( f_global_titles  , gtb ) ){
                    if(debg)qDebug()<< "WAND:CREDENTIALS: !!! CANNOT SAVE FAVS - "<< f_global_titles;
                }
            } else {
                QByteArray gtb = readFile( f_global_titles );
                gt = QJsonDocument::fromBinaryData( gtb );
            }
            int tcnt = gt.array().count();
            qDebug()<<"TTTIT:"<<*titles<<titcnt;
            for( int i = 0;i< titcnt;i++ ){
                QByteArray titati=titles->at( i ).toObject().value( "filename" ).toString().toLatin1();
                                qDebug()<<"TITLE!!!!:"<<titles->count()<<i<<titles->at(i)<<titati;
                int title_id = titles->at( i ).toObject().value( "id" ).toInt();
                title_media[i] = new QByteArray(titles->at( i ).toObject().value( "filename" ).toString().toLatin1());
                title_oid[i] = titles->at( i ).toObject().value( "oid" ).toInt();
                int cnt = 0;
                for( int j = 0;j< tcnt;j++ ){
                    if( title_id == gt.array().at( j ).toObject().value( "title_id" ).toString().toInt() ){
                        cnt = gt.array().at( j ).toObject().value( "count" ).toInt();
                        title_items[i] = cnt;
                        timeline_count_by_title_id[title_id] = cnt;

                        break;
                    }
                }
            }
            emit got_titles( *titles );
            if(debg)qDebug()<<"TITLES:"<<titles;
        }
    } );
}
void Credentials::get_favs( bool write ){
    get_favs( ""  , write );
}
void Credentials::get_favs( QString type  , bool write ){
    //  if(debg)qDebug()<<"GET FANS:"<<type<<write;
    get_fav_videos_list( write );
    if( !offline_m ){
        if( type == "fav_items" || type == "" ){
            jfi = get_fav_items();
            if( jfi.array().count() != 0 && write ){
                QByteArray bfi = jfi.toBinaryData();
                if( !writeFile( f_user_fav_i  , bfi ) ){
                    if(debg)qDebug()<< "WAND:CREDENTIALS: !!! CANNOT SAVE FAVS - "<< f_user_fav_s;
                }
            }
        }
        if( type == "fav_scenes" || type == "" ){
            jfs = get_fav_scenes();
            if( jfs.array().count() != 0 && write ){
                QByteArray bfs = jfs.toBinaryData();
                if( !writeFile( f_user_fav_s  , bfs ) ){
                    if(debg)qDebug()<< "WAND:CREDENTIALS: !!! CANNOT SAVE FAVS - "<< f_user_fav_s;
                }
            }
        }
        if( type == "fav_videos" || type == "" ){
            jfm = get_fav_videos();
            if( jfm.array().count() != 0 && write ){
                QByteArray bfm = jfm.toBinaryData();
                if( !writeFile( f_user_fav_m  , bfm ) ){
                    if(debg)qDebug()<< "WAND:CREDENTIALS: !!! CANNOT SAVE FAVS - "<< f_user_fav_m;
                }
            }
        }
    } else {
        if(debg)qDebug()<< "WAND:CREDENTIALS: !!! OFFLINE !!! ";
        QByteArray bfi = readFile( f_user_fav_i );
        QByteArray bfs = readFile( f_user_fav_s );
        QByteArray bfm = readFile( f_user_fav_m );
        jfi = QJsonDocument::fromBinaryData( bfi );
        jfs = QJsonDocument::fromBinaryData( bfs );
        jfm = QJsonDocument::fromBinaryData( bfm );
    }
    emit got_favs();
}
QJsonDocument Credentials::get_fav_items(){
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "fav_items" );
    sql_api.add_field( "*" );
    sql_api.add_equ( "user_id"  , curr_id );
    sql_api.set_sort( "time" );
    QList< QByteArray> reply = wait_and_get_reply( pull_query( sql_api.get_query() ) );
    QJsonDocument r;
    if( !reply.empty() ){
        r = QJsonDocument::fromJson( reply.takeFirst() );
    }
    return r;
}
QJsonDocument Credentials::get_fav_scenes(){
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "fav_scenes" );
    sql_api.add_field( "*" );
    sql_api.add_equ( "user_id"  , curr_id );
    sql_api.set_sort( "time" );
    QList< QByteArray> reply = wait_and_get_reply( pull_query( sql_api.get_query() ) );
    QJsonDocument r;
    if( !reply.empty() ){
        r = QJsonDocument::fromJson( reply.takeFirst() );
    }
    return r;
}
QJsonDocument Credentials::get_fav_videos(){
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "titles  , fav_videos" );
    sql_api.add_field( "fav_videos.id  , titles.id  , titles.title  , titles.filename  , titles.duration" );
    sql_api.add_equ( "fav_videos.user_id"  , curr_id );
    sql_api.add_equ_np( "fav_videos.title_id"  , "titles.id" );
    QList< QByteArray> reply = wait_and_get_reply( pull_query( sql_api.get_query() ) );
    QJsonDocument r;
    if( !reply.empty() ){
        r = QJsonDocument::fromJson( reply.takeFirst() );
    }
    return r;
}
QJsonDocument Credentials::get_fav_videos_list( bool write ){
    QJsonDocument res;
    if( write ){
        sql_api.set_method( sqlApi::SELECT );
        sql_api.add_table( "fav_videos" );
        sql_api.add_field( "title_id" );
        sql_api.add_equ( "user_id"  , curr_id );
        QList< QByteArray> reply = wait_and_get_reply( pull_query( sql_api.get_query() ) );
        if( !reply.empty() ){
            fvl = QJsonDocument::fromJson( reply.takeFirst() );
        }
        QByteArray bfml = fvl.toBinaryData();
        if( !writeFile( f_user_fav_ml  , bfml ) ){
            if(debg)qDebug()<< "WAND:CREDENTIALS: !!! CANNOT SAVE FAVS ML - "<< f_user_fav_ml;
        }
    } else {
        QByteArray bfml = readFile( f_user_fav_ml );
        fvl = QJsonDocument::fromBinaryData( bfml );
    }
    return fvl;
}
int Credentials::check_duplicate( QString fields  , QString tabl  , QString cond ){
    QJsonObject rarr;
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( tabl );
    sql_api.add_field( fields );
    sql_api.add_cond( cond );
    QList< QByteArray> reply = wait_and_get_reply( pull_query( sql_api.get_query() ) );
    if( !reply.empty() ){
        QJsonDocument d = QJsonDocument::fromJson( reply.takeFirst() );
        return geti( d  , "id" );
    } else return false;
}
bool Credentials::check_item_favs( QString image ){
    if( check_duplicate( "*"  , "fav_items"  , "user_id = '"+QString::number( curr_id )+"' and image = '"+image+"'" )>0 )
        return true;
    else return false;
}
bool Credentials::check_scene_favs( QString image ){
    if( check_duplicate( "*"  , "fav_scenes"  , "user_id = '"+QString::number( curr_id )+"' and frame = '"+image+"'" )>0 )
        return true;
    else return false;
}
void Credentials::remove_from_favs( int id  , QString table  , QString sender ){
    if( table == "videos" )table = "fav_videos";
    sql_api.set_method( sqlApi::DELETE );
    sql_api.add_table( table );
    if( table == "fav_scenes" ){
        if( sender == "carousel" ){
            //      if(debg)qDebug()<< "WAND:CREDENTIALS:CAROUSEL DELETE SCENE:"<< id;
            sql_api.add_equ( "sid"  , id );
        } else {
            //       if(debg)qDebug()<< "WAND:CREDENTIALS:???????????? DELETE SCENE:"<< id;
            sql_api.add_equ( "id"  , id );
            emit check_carousel_displayed_fav( 0  , id );
        }
    }
    if( table == "fav_items" ){
        if( sender == "carousel" ){
            //    if(debg)qDebug()<< "WAND:CREDENTIALS:CAROUSEL DELETE ITEM:"<< id;
            sql_api.add_equ( "iid"  , id );
        } else {
            //   if(debg)qDebug()<< "WAND:CREDENTIALS:???????????? DELETE ITEM:"<< id;
            sql_api.add_equ( "id"  , id );
            emit check_carousel_displayed_fav( 1  , id );
        }
    }
    if( table == "fav_videos" ){
        sql_api.add_equ( "title_id"  , id );
    }
    sql_api.add_equ( "user_id"  , curr_id );
    videos_to_unmark = id;
    wait_and_get_reply( pull_query( sql_api.get_query() ) );
    get_favs( table  , true );
}
void Credentials::add_fav_item( int id  , QString title  , QString event  , QString name  , QString image  , int time ){
    //    if(debg)qDebug()<< "WAND:CREDENTIALS:add_fav_item"<< id;
    //    if( !check_duplicate( "iid"  , "fav_items"  , "iid = '"+QString::number( id )+"' and user_id = '"+QString::number( curr_id )+"'" ) ){
    //        if(debg)qDebug()<< "WAND:CREDENTIALS:NO DUPLICATES add fav item";
    sql_api.set_method( sqlApi::INSERT );
    sql_api.add_table( "fav_items" );
    sql_api.add_default();
    sql_api.add_insdata( curr_id );
    sql_api.add_insdata( title );
    sql_api.add_insdata( event );
    sql_api.add_insdata( name );
    sql_api.add_insdata( image );
    sql_api.add_instime( time );
    sql_api.add_insDate();
    sql_api.add_insdata( id );
    wait_and_get_reply( pull_query( sql_api.get_query() ) );
    get_favs( "fav_items"  , true );
    //     } else if(debg)qDebug()<< "WAND:CREDENTIALS:ITEM:"<< id<< " exist as fav";
}
void Credentials::add_fav_scene( int id  , QString title  , QString event  , QString scene  , int time ){
    QJsonObject rarr;
    //    if( !check_duplicate( "sid"  , "fav_scenes"  , "sid = '"+QString::number( id )+"' and user_id = '"+QString::number( curr_id )+"'" ) ){
    sql_api.set_method( sqlApi::INSERT );
    sql_api.add_table( "fav_scenes" );
    sql_api.add_default();
    sql_api.add_insdata( curr_id );
    sql_api.add_insdata( title );
    sql_api.add_insdata( event );
    sql_api.add_insdata( scene );
    sql_api.add_instime( time );
    sql_api.add_insDate();
    sql_api.add_insdata( id );
    wait_and_get_reply( pull_query( sql_api.get_query() ) );
    get_favs( "fav_scenes"  , true );
    //      } else if(debg)qDebug()<< "WAND:CREDENTIALS:SCENE:"<< id<< " exist as fav";
}
void Credentials::add_fav_videos( int id ){
    if(debg)qDebug()<< "Addfav video";
    sql_api.set_method( sqlApi::INSERT );
    sql_api.add_table( "fav_videos" );
    sql_api.add_default();
    sql_api.add_insdata( curr_id );
    sql_api.add_insdata( id );
    wait_and_get_reply( pull_query( sql_api.get_query() ) );
    get_favs( "fav_videos"  , true );
}
QByteArray Credentials::readFile( QString _name ){
    QFileInfo _n( _name );
    QFile f( _name );
    if( !f.open( QIODevice::ReadOnly ) ){
        if( f.error() != 5 )
            if(debg)qDebug()<< "WAND:CREDENTIALS:"<< f.error()<< f.errorString();
        f.close();
        return "";
    }
    const QByteArray _content = f.readAll();
    f.close();
    if( _content.isEmpty() )if(debg)qDebug()<< "WAND:CREDENTIALS: --- file "<< _name<< " is empty";
    return _content;
}
QString Credentials::getFile( QString _name ){
    QFileInfo _n( _name );
    QFile f( _name );
    if( !f.open( QIODevice::ReadOnly ) ){
        if(debg)qDebug()<< "WAND:CREDENTIALS: !!! FILE "<< _name<< " CANNOT BE OPENED FOR WRITE";
        f.close();
        return "";
    }
    const QString _content = f.readAll();
    f.close();
    return _content;
};
bool Credentials::writeFile( QString _name  , const QByteArray _fcont ){
    QFile f( _name );
    if( !f.open( QIODevice::ReadWrite ) ){
        if(debg)qDebug()<< "WAND:CREDENTIALS: !!! FILE "<< _name<< f.fileName()<< " CANNOT BE WRITTEN";
        inform( "Cannot access storage." );
        return false;
    } else {
        f.write( _fcont );
        f.close();
        return true;
    }
}
void Credentials::get_params(){
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "apk_params" );
    sql_api.add_field( "value" );
    sql_api.add_equ( "name"  , "player_offset" );
    QUrl query = sql_api.get_query();
    int qid = pull_query( query );
    QJsonDocument r = is_data( wait_and_get_reply( qid ) );
    if( !r.isEmpty() ){
        player_offset = gets( r  , "value" ).toInt();
        if(debg)qDebug()<< "Player offset:"<< player_offset;
    }
}
Credentials::~Credentials(){
    if(debg)qDebug()<<"CREDENTIALS DESTR";
    //  manager->deleteLater();
    //  sql_api.deleteLater();
    if(debg)qDebug()<< "WAND:CREDENTIALS:~Credentials()";
}
