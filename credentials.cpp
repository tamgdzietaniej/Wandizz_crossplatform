#include "credentials.h"
Credentials::Credentials( QObject *parent ): QObject( parent )  ,
    lat( "" )  ,
    lon( "" )  ,
    offline_m( false )  ,
    recovery( false )  ,
    hsh( "" )  ,
    api_ind( 1 )  , recov( false ), social_type(0), debg(false),
    context_unlocked(false),context_id(-1)
{
    make_dir( hash_dir );
    make_dir( d_carousel );
    make_dir( d_recorder );
    make_dir( d_posters );
    make_dir( d_logos );
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
/* I:N:I:T: */
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
    jfi = QJsonArray();
    jfs = QJsonArray();
    jfv = QJsonArray();
    get_remote_params();
    get_stats(true);
    get_user_data();
}

/* THREADED POOLED F-I-F-O NETWORK GRABBER */
/* reuester */
int Credentials::make_request( QString ident ){
    QNetworkRequest* request=new QNetworkRequest;
    request->setAttribute( QNetworkRequest::User  , api_ind );
    request->setUrl( api_url+ident );
    manager->get( *request );
    return api_ind++;
}
/* requester for sql-api */
int Credentials::pull_query( QUrl url ){
    QNetworkRequest* request=new QNetworkRequest;
    request->setAttribute( QNetworkRequest::User  , api_ind );
    request->setRawHeader( "Keep-Alive"  , "timeout = 5  , max = 200;" );
    request->setRawHeader( "Cache-Control"  , "no-cache" );
    request->setRawHeader( "Cache-Control"  , "no-store" );
    request->setAttribute( QNetworkRequest::HttpPipeliningAllowedAttribute  , true );
    request->setUrl( url );
    manager->get( *request );
    return api_ind++;
}
/* do threaded  EVENTLOOP until reply appear */
QList< QByteArray> Credentials::wait_and_get_reply( int id ){
    QList< QByteArray> reply;
    while( reply.empty() ){
        reply = get_reply( id );
        QApplication::processEvents();
    }
    return reply;
}
/* replies collector */
void Credentials::collect_replies( QNetworkReply* reply ){
    replies.append( reply );
}
/* reply arrived */
QList< QByteArray> Credentials::get_reply( int id ){
    QList< QByteArray> reply;
    reply.clear();
    int rc = replies.count();
    for( int i = 0;i< rc;i++ ){
        if( replies.at( i )->request().attribute( QNetworkRequest::User ).toInt() == id ){
            QByteArray rep=replies.at(i)->readAll();
            reply.append( rep );
            if(debg){
                qDebug()<<"************ REPLY BEGIN ****************";
                QJsonArray ja(QJsonDocument::fromJson(rep).array());
                for(int i=0;i<ja.count();i++){
                    qDebug().noquote()<<ja.at(i);
                    if(i>10)break;
                }
                qDebug()<<"************ REPLY END ****************";
                qDebug()<<"";
            }
            // if(debg)qDebug()<<"CRED:GOT REPLY:"<<rep;
            replies.takeAt(i)->deleteLater();
            break;
        }
    }
    return reply;
}

/* some json parsing stuff */
int Credentials::geti( QJsonArray doc  , QString key  , int i ){
    if( !doc.count() )return 0;
    QJsonValue val;
    val = doc.at( i );
    val = val.toObject().take( key );
    if( val.isString() )return val.toString().toInt();
    else return val.toInt();
}
QString Credentials::gets( QJsonArray doc  , QString key  , int i ){
    if( !doc.count() )return "";
    QJsonValue val;
    val = doc.at( i );
    val = val.toObject().take( key );
    return val.toString();
}
/*
 * USER AUTHORIZATON USER AUTHORIZATON USER AUTHORIZATON USER AUTHORIZATON USER AUTHORIZATON USER AUTHORIZATON USER AUTHORIZATON USER AUTHORIZATON *
 */

void Credentials::get_user_data(){
    if( !check_signature() ){
        if( !offline_m ){
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
    if(set_user(fetch_remote())){
        emit got_user_data();
    }
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
void Credentials::get_user_data( QString login  , QString pass, int social){
    //  if(debg)qDebug()<< "GET USER DATA:"<< login<< pass;
    recovery = true;
    social_type=social;
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "users" );
    sql_api.add_field( "*" );
    sql_api.add_equ( "name"  , login );
    if(social==0)sql_api.add_pass_equ( "pass"  , pass );
    if(!set_user(fetch_remote()))
        emit rec_acc_not_found();
}
bool Credentials::set_user(QJsonArray r){
    if( !r.isEmpty() ){
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
    return r;
}
void Credentials::get_remote_params(){
    //  if(debg)qDebug()<< "GET USER DATA:"<< login<< pass;
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "appsettings" );
    sql_api.add_field( "var,val" );
    sql_api.add_equ( "user"  , "all_users");
    QJsonArray ar = fetch_remote();
    for(int i=0;i<ar.count();i++){
        QJsonObject json(ar.at(i).toObject());
        QString val=json.value("val").toString();
        QString var=json.value("var").toString();
        qDebug()<<val<<" ===> "<<var;
        if(var=="carousel_video_duration")prospect_settings.player_offset=val.toInt();
        if(var=="carousel_video")prospect_settings.player_enabled=val.toInt();
        if(var=="show_prospect_favs")prospect_settings.prospect_favs=val.toInt();
        if(var=="show_prospect_shares")prospect_settings.prospect_shares=val.toInt();
        if(var=="show_toggled_star")prospect_settings.prospect_osd=val.toInt();
        if(var=="restrict_if_no_context")prospect_settings.menu_context_restrict=val.toInt();
        if(var=="prospect_initial_view")prospect_settings.prospect_initial_view=val.toInt();
        if(var=="prospect_filter_chars")prospect_settings.prospect_filter_chars=val.toInt();
        if(var=="widget_height_factor"){
            prospect_settings.widget_height_factor=val.toDouble();
        }
        if(var=="search_switch_with_top_bar")prospect_settings.search_switch_with_top_bar=val.toInt();
        if(var=="videos_coming_soon_picture_dim")prospect_settings.videos_coming_soon_picture_dim=val.toInt();
        if(var=="videos_coming_soon_text")prospect_settings.videos_coming_soon_text=val.toUtf8();
        if(var=="videos_coming_soon_text_size")prospect_settings.videos_coming_soon_text_size=val.toInt();
        if(var=="search_mag_glass")prospect_settings.search_mag_glass=val.toInt();
        if(var=="search_text_placeholder")prospect_settings.search_text_placeholder=val.toUtf8();
        if(var=="search_text_color")prospect_settings.search_text_color=val;
        if(var=="search_field_color")prospect_settings.search_field_color=val;
        if(var=="search_height_factor")prospect_settings.search_height_factor=val.toInt();
        qDebug()<<prospect_settings.search_field_color;
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
    sql_gps.set_method( sqlApi::INSERT );
    sql_gps.add_table( "trip" );
    sql_gps.add_insdata( curr_id );
    sql_gps.add_insdata( lat );
    sql_gps.add_insdata( lon );
    sql_gps.add_insDate();
    sql_gps.send_query();
    qDebug()<<"LAT<LON:"<<lat<<lon;
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
        QJsonArray r = fetch_remote();
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
    QJsonArray r= fetch_remote();
    if( r.count() ){
        curr_id = geti( r  , "lastinsertid" );
        writeFile( f_user_login  , login.toLatin1() );
        writeFile( f_user_nick  , nick.toLatin1() );
        writeFile( f_user_id  , QString::number( curr_id ).toLatin1() );
        if(debg)qDebug()<< "ADDED:"<< curr_id;
        update_user_activity( "New user added" );
        return true;
    } else return false;
}
QByteArray Credentials::is_raw_data( QList< QByteArray> r ){
    return r.takeFirst();
}
int Credentials::check_login( QString login ){
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "users" );
    sql_api.add_field( "*" );
    sql_api.add_equ( "name"  , login );
    QJsonArray r = fetch_remote();
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
void Credentials::prepare_titles(){
    int titcnt = jtit.count();
    int tcnt = jtc.count();
    for( int i = 0;i< titcnt;i++ ){
        QByteArray titati=jtit.at( i ).toObject().value( "filename" ).toString().toLatin1();
        int title_id = jtit.at( i ).toObject().value( "id" ).toString().toInt();
        title_media[i] = new QByteArray(jtit.at( i ).toObject().value( "filename" ).toString().toLatin1());
        title_oid[i] = jtit.at( i ).toObject().value( "oid" ).toString().toInt();
        int cnt = 0;
        for( int j = 0;j< tcnt;j++ ){
            if( title_id == jtc.at( j ).toObject().value( "title_id" ).toString().toInt() ){
                cnt = jtc.at( j ).toObject().value( "count" ).toInt();
                title_items[i] = cnt;
                timeline_count_by_title_id[title_id] = cnt;
                break;
            }
        }
    }
}
void Credentials::get_favs( bool write ){
    get_favs( ""  , write );
}
/* GET ALL FAVS MACRO */
void Credentials::get_favs( QString type  , bool write ){
    if( !offline_m && write){
        if( type == "fav_items" || type == "" ){
            jfi = get_fav_items();
            updateFileCache(jfi,f_user_fav_i);
        }
        if( type == "fav_scenes" || type == "" ){
            jfs = get_fav_scenes();
            updateFileCache(jfs,f_user_fav_s);
        }
        if( type == "fav_videos"  || type=="videos" || type == "" ){
            jfv = get_fav_videos();
            updateFileCache(jfv,f_user_fav_v);
        }
    } else {
        jfi = getFromFileCache(f_user_fav_i);
        jfs = getFromFileCache(f_user_fav_s);
        jfv = getFromFileCache(f_user_fav_v);
    }
}
/* FILECACHE IN-OUT */
void Credentials::updateFileCache(const QJsonArray &json, const QString &path, bool w){
    if( json.count() != 0 && w ){
        QByteArray bfi(QJsonDocument(jfi).toBinaryData());
        if( !writeFile( f_user_fav_i  , bfi ) ){
            if(debg)qDebug()<< "WAND:CREDENTIALS: !!! CANNOT SAVE"<< f_user_fav_s;
        }
    }
}
QJsonArray Credentials::getFromFileCache(const QString &s){
    QByteArray b = readFile( s );
    return QJsonDocument::fromBinaryData( b ).array();}

/* FAVOURITES API */
/* fav items */
QJsonArray Credentials::get_fav_items(){
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "favitems, titles, items2" );
    sql_api.add_field( "items2.assigned,items2.id,items2.name,items2.image,titles.id,titles.title,titles.filename,titles.oid" );
    sql_api.add_equ( "user_id"  , curr_id );
    sql_api.add_equ_np("favitems.fav_id","items2.id");
    sql_api.add_equ_np("titles.id","items2.assigned");
    if(context_id!=-1){
        sql_api.add_equ( "items.oid"  , QString::number(context_id) );
    }
    return  fetch_remote(true);
}
/* fav scenes */
QJsonArray Credentials::get_fav_scenes(){
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "favscenes,titles,scenes,timeline" );
    sql_api.add_field( "favscenes.fav_id, titles.title,scenes.sceneid,scenes.scene_image,timeline.start,timeline.scene_id" );
    sql_api.add_equ( "user_id"  , curr_id );
    sql_api.add_equ_np("favscenes.fav_id","scenes.sceneid");
    sql_api.add_equ_np("favscenes.fav_id","timeline.scene_id");
    sql_api.add_equ_np("scenes.title_id","titles.id");
    if(context_id!=-1){
        sql_api.add_equ( "oid"  , QString::number(context_id) );
    }
    sql_api.set_sort( "timeline.start" );
    return fetch_remote(true);
}
/*fav  videos */
QJsonArray Credentials::get_fav_videos( ){
    QJsonDocument res;
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "titles,favvideos" );
    sql_api.add_field( "*" );
    sql_api.add_equ( "user_id"  , curr_id );
    sql_api.add_equ_np( "fav_id"  , "titles.id");
    if(context_id!=-1){
        sql_api.add_equ( "oid"  , context_id );
    }
    jfv=fetch_remote();
    return jfv;
}


/* COMPLETE TABLES OF: */
void Credentials::get_stats(bool write){
    if(write){
        jp=get_producers();
        jtl=get_timeline();
        jtit=get_titles();
        jtc=getEachTitleContentCounter();
        updateFileCache(jp,f_producers);
        updateFileCache(jtl,f_timeline);
        updateFileCache(jtc,f_titles_ev_cnt);
    } else {
        jp=getFromFileCache(f_producers);
        jtl=getFromFileCache(f_titles_ev_cnt);
    }
    prepare_titles();
}
/* producers */
QJsonArray Credentials::get_producers(){
    sql_api.set_method(sqlApi::SELECT);
    sql_api.add_table("producers");
    sql_api.add_field("pid, name, others");
    return fetch_remote();
}
/* timeline counter */
QJsonArray Credentials::getEachTitleContentCounter(){
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "timeline" );
    sql_api.add_field( "title_id" );
    sql_api.add_field( "COUNT( * )AS count" );
    sql_api.set_groupby( "title_id" );
    return fetch_remote();
}
/* timeline */
QJsonArray Credentials::get_timeline(){
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( "timeline" );
    sql_api.add_field( "*" );
    return fetch_remote();
}
/* titles */
QJsonArray Credentials::get_titles(){
    sql_api.set_method(sqlApi::SELECT);
    sql_api.add_table("titles,producers");
    sql_api.add_field("*");
    sql_api.add_nequ("id",0);
    sql_api.add_equ_np("oid","pid");
    if(context_id!=-1){
        sql_api.add_equ( "oid"  , context_id);
    }
    sql_api.set_sort("name asc");
    return fetch_remote();
}
QJsonArray Credentials::fetch_remote(bool sh){
    QList<QByteArray> ls=wait_and_get_reply( pull_query( sql_api.get_query(sh) ));
    QJsonDocument r;
    r={};
    if(!ls.empty())r=QJsonDocument::fromJson( ls.takeFirst() );
    return r.array();
}
int Credentials::check_duplicate( QString fields  , QString tabl  , QString cond ){
    QJsonObject rarr;
    sql_api.set_method( sqlApi::SELECT );
    sql_api.add_table( tabl );
    sql_api.add_field( fields );
    sql_api.add_cond( cond );
    QJsonArray d=fetch_remote();
    if(!d.isEmpty())
        return geti( d  , "id" );
    else return false;
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

void Credentials::del_fav( const QString& tab,int id ){
    if(debg)qDebug()<< "Addfav "<<tab;
    sql_api.set_method( sqlApi::DELETE );
    sql_api.add_table( tab );
    sql_api.add_equ("fav_id",id);
    sql_api.add_equ("user_id",curr_id);
    sql_api.add_insdata( id );
    fetch_remote();
    emit got_favs();
}
void Credentials::add_fav(const QString& tab,int id ){
    if(debg)qDebug()<< "Addfav "<<tab;
    sql_api.set_method( sqlApi::INSERT );
    sql_api.add_table( tab );
    sql_api.add_default();
    sql_api.add_insdata( curr_id );
    sql_api.add_insdata( id );
    fetch_remote();
    emit got_favs();
}
void Credentials::add_fav_video( int id ){
    add_fav("favvideos",id);
}
void Credentials::add_fav_item( int id  ){
    add_fav("favitems",id);
}
void Credentials::add_fav_scene( int id  ){
    add_fav("favscenes",id);
}
void Credentials::del_fav_video( int id ){
    del_fav("favvideos",id);
}
void Credentials::del_fav_item( int id  ){
    del_fav("favitems",id);
}
void Credentials::del_fav_scene( int id  ){
    del_fav("favscenes",id);
}
void Credentials::toggle_fav_video(int tid,bool s){
    if(s)add_fav_video(tid);
    else del_fav_video(tid);
}
void Credentials::toggle_fav_scene(int sid,bool s){
    if(s)add_fav_scene(sid);
    else del_fav_scene(sid);
}
void Credentials::toggle_fav_item(int iid,bool s){
    if(s)add_fav_video(iid);
    else del_fav_item(iid);
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
Credentials::~Credentials(){
    if(debg)qDebug()<<"CREDENTIALS DESTR";
    //  manager->deleteLater();
    //  sql_api.deleteLater();
    if(debg)qDebug()<< "WAND:CREDENTIALS:~Credentials()";
}
