#include "sqlapi.h"

sqlApi::sqlApi(QObject *parent) : QObject(parent)
{
    clear_all();
    manager=new QNetworkAccessManager();
}
void sqlApi::selectQuery(QStringList tables, QStringList filters, QStringList conditions, QStringList format){
    set_method( SELECT );
    while(!tables.empty()){
        add_table(tables.takeFirst());
    }
    while(!filters.empty()){
        add_field(filters.first());
    }
    while(!conditions.empty()){
        add_cond(conditions.takeFirst());
    }
    if(!format.empty())
        set_groupby(format.takeFirst());
    if(!format.empty())
        set_sort(format.takeFirst());
    qDebug()<<"SELECT BUILDER:"<<get_query();
}
void sqlApi::clear_all(){
    method=NONE;
    clear_data();

}
void sqlApi::clear_data(){
    tables.clear();
    fields.clear();
    conds.clear();
    query.clear();
    insdata.clear();
    updata.clear();
    sort.clear();
    groupby="";
    limit="";
    sq="";
}
void sqlApi::add_table(QString table){
    if(tables.length()>0)tables.append(",");
    tables.append(table);
}
void sqlApi::add_field(QString field){
    if(fields.length()>0)fields.append(",");
    fields.append(field);

}
void sqlApi::add_cond(QString cond){
    if(!conds.isEmpty())cond.append(" AND ");
    conds.append(cond);
}
void sqlApi::add_or(){
    conds.append(" OR ");
}
void sqlApi::add_and(){
    conds.append(" AND ");
}

void sqlApi::add_updata(QString key,QString val,bool crypto){
    if(updata.length()>0)updata.append(",");
    if(crypto){
        updata.append(key+"=PASSWORD('"+val+"')");
    } else {
        updata.append(key+"='"+val+"'");
    }
}
void sqlApi::add_updata(QString key,QString val){
    add_updata(key,val,false);
}
void sqlApi::add_insdata(QString val){
    add_insdata(val,false);
}
void sqlApi::add_limit(int limit){

}
void sqlApi::add_insdata(QString val,bool crypto){
    if(insdata.length()>0)insdata.append(",");
    if(crypto){
        insdata.append("PASSWORD('"+val+"')");
    } else {
        insdata.append("'"+val+"'");
    }
}
void sqlApi::add_insdata(int val){
    add_insdata(QString::number(val));
}
void sqlApi::set_groupby(QString gb){
    groupby=gb;
}
void sqlApi::add_instime(int t){
    QTime time;
    time.setHMS(0,0,0);
    QString stime=time.addMSecs(t).toString("hh:mm:ss");
    add_insdata(stime);
}
void sqlApi::add_insDate(){
    QDate date;
    add_insdata(date.currentDate().toString("yyyy-MM-dd")+" "+QTime::currentTime().toString("hh:mm:ss"));
}
/* do kasacji 10.08.2019
void sqlApi::add_equ(QString var,QByteArray val){
    if(!conds.isEmpty())conds.append(" AND ");
    conds.append(var+"='"+QUrl::toPercentEncoding(val)+"'");
}
*/
void sqlApi::add_equ(QString var,QString val){
    if(!conds.isEmpty())conds.append(" AND ");
    conds.append(var+"='"+val+"'");
}
void sqlApi::add_nequ(QString var,QString val){
    if(!conds.isEmpty())conds.append(" AND ");
    conds.append(var+"<>'"+val+"'");
}
void sqlApi::add_nequ(QString var,int val){
    if(!conds.isEmpty())conds.append(" AND ");
    conds.append(var+"<>"+QString::number(val));
}
void sqlApi::add_equ_np(QString var,QString val){
    if(!conds.isEmpty())conds.append(" AND ");
    conds.append(var+"="+val);
}
void sqlApi::add_equ(QString var,int val){
    if(!conds.isEmpty())conds.append(" AND ");
    conds.append(var+"='"+QString::number(val)+"'");
}
void sqlApi::add_pass_equ(QString var,QString val){
    if(!conds.isEmpty())conds.append(" AND ");
    conds.append(var+"=PASSWORD('"+val+"')");
}
void sqlApi::add_default(){
    if(insdata.length()>0)insdata.append(",");
    insdata.append("DEFAULT");
}
void sqlApi::set_sort(QString srt){
    sort=srt;
}
void sqlApi::set_method(Methods meth){
    clear_all();
    method=meth;
}
/*
QString sqlApi::getv(QJsonDocument d,QString key){
    return getv(d,key,0);
}

QString sqlApi::getv(QJsonDocument d,QString key, int i){
    QJsonValue val;
    obj=rarr(d).at(i).toObject();
    val=obj.value(key);
    return val.toString();
}
int sqlApi::igetv(QJsonDocument d,QString key){
    QString s;
    s=getv(d,key,0);
    return s.toInt();
}
int sqlApi::igetv(QJsonDocument d,QString key,int i){
    QString s;
    s=getv(d,key,i);
    return s.toInt();
}
QJsonArray sqlApi::rarr(QJsonDocument d){
    return d.array();
}
QList<int> sqlApi::get_int_arr_by_key(QJsonDocument d,QString key){
    //  qDebug()<<"GET INT ARR:"<<key<<","<<rarr();
    QList<int> arr;
    arr.clear();
    for(int i=0;i<rarr(d).count();i++){
        arr.append(rarr(d).at(i).toObject().value(key).toString().toInt());
    }
    return arr;
}
QStringList sqlApi::get_arr_by_key(QJsonDocument d,QString key){
    QStringList arr;
    arr.clear();
    for(int i=0;i<rarr(d).count();i++){
        arr.append(rarr(d).at(i).toObject().value(key).toString());
    }
    return arr;
}
int sqlApi::affected_rows(QJsonDocument d){
    QJsonObject res = doc.array().at(0).toObject();
    if(res.contains("affected_rows")){
        return getv(d,"affected_rows").toInt();
    }  else {
        return false;
    }
}
*/
    void sqlApi::set_limit(int l){
    if(l>0)limit=QString::number(l);
    else limit="";
}
QUrl sqlApi::get_query(){
    query="met="+meths.at(method);
    //query.append("&wht="+QUrl::toPercentEncoding(fields));
    query.append("&wht="+fields);
    query.append("&tab="+QUrl::toPercentEncoding(tables));
    query.append("&uval="+QUrl::toPercentEncoding(updata));
    query.append("&ival="+QUrl::toPercentEncoding(insdata));
    //query.append("&con="+QUrl::toPercentEncoding(conds));
    query.append("&con="+conds);
    query.append("&sort="+QUrl::toPercentEncoding(sort));
    query.append("&limit="+QUrl::toPercentEncoding(limit));
    query.append("&gb="+QUrl::toPercentEncoding(groupby));
    query.prepend(root_url);
    return query;
}
void sqlApi::send_query(){
    return send_query(false);
}
void sqlApi::send_query(bool show_query){
    get_query();
    QNetworkRequest req;
    req.setUrl(QUrl(query));
    req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute,true);
    req.setRawHeader("Connection","Keep-Alive");
    req.setRawHeader("Keep-Alive","timeout=30, max=100;");
    QNetworkReply* const reply=manager->get(req);
    connect(reply,&QNetworkReply::finished,[reply](){
        if(qApp) reply->deleteLater();
        else delete reply;
    });
    connect(qApp,&QApplication::aboutToQuit,reply,[reply](){reply->abort();});
      qDebug()<<"QUery downloader ------------------------------------ \n"<<query<<"\n"<<resp<<"\n-------------------";
    if(show_query){
        qDebug()<<"QUERY::"<<query;
    }
}
void sqlApi::foo_reply(QNetworkReply *rep){
    qDebug()<<"SQL:"<<rep->errorString();
}
sqlApi::~sqlApi(){
    manager->deleteLater();
    qDebug()<<"sql destr";
}
