#ifndef SQLAPI_H
#define SQLAPI_H
#include "globals.h"

class sqlApi : public QObject
{
    Q_OBJECT
public:
    enum Methods{NONE,SELECT, INSERT, UPDATE, DELETE} method=NONE;

    explicit sqlApi(QObject *parent = nullptr);
    ~sqlApi();
    void selectQuery(QStringList table, QStringList filters, QStringList conditions={},QStringList format={});
    QString limit,sq;
    void clear_all();
    void clear_data();
    bool debg;
    void add_limit(int);
    void set_method(Methods);
    void add_table(QString);
    void add_field(QString);
    void add_cond(QString);
    void add_updata(QString,QString);
    void add_updata(QString,QString,bool);
    void add_insdata(QString);
    void add_insdata(int);
    void add_insdata(QString,bool);
    void set_limit(int);
    void set_groupby(QString);
    QString groupby;
    QByteArray resp;
    void add_instime(int);
    void add_instime();
    void add_insDate();
    void add_default();
    void add_or();
    void add_and();
    void add_equ(QString,QString);
    void add_equ_np(QString,QString);
    //void add_equ(QString,QByteArray);
    void add_pass_equ(QString,QString);
    void add_equ(QString,int);
    void add_nequ(QString,QString);
    void add_nequ(QString,int);
    void set_sort(QString);
    void cond(QString);
    bool equ_added;
    QString query;
    void mfree();
    int apicnt;
    void send_query();
    void send_query(bool);
    QUrl get_query();
private:
    QNetworkAccessManager* manager;
    QString tables;
    QString fields;
    QString conds;
    QString updata;
    QString insdata;
    QString sort;
    QStringList meths{"","sel","ins","upd","del"};
    QString root_url="https://wandizz.com/acr/api/uapi.php?";
signals:
    void offline_mode(bool);
public slots:
    void foo_reply(QNetworkReply*);
};

#endif // SQLAPI_H
