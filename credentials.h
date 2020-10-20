#ifndef CREDENTIALS_H
#define CREDENTIALS_H
#include "globals.h"
#include "sqlapi.h"
#include "downloaderStd.h"
#include "permissions.h"

class Credentials : public QObject
{
    Q_OBJECT
public:
    explicit Credentials(QObject *parent = nullptr);
    ~Credentials();
    int curr_id;
    QString curr_user;
    QJsonDocument fvl;
    bool update_loc();
    sqlApi sql_api;
    bool recov;
    int social_type;
    QJsonDocument is_data(QList<QByteArray>);
    QNetworkAccessManager* manager,*upmanager;
    QJsonDocument jfi,jfs,jfm,gt;
    QJsonArray titles;
    QFile* file;

    bool update_user_activity(QString);
    void get_titles();
    bool check_signature();
    void make_dir(QString);
    QString s_curr_id,curr_pass,curr_join;
    QString curr_nick="";
    QString curr_phone="enter phone number";
    void update_user(QString,QString,QString,QString);
    int check_user(QString);
    QJsonDocument get_fav_items(QString);
    QJsonDocument get_fav_scenes(QString);
    QJsonDocument get_fav_videos_list(bool);
    QJsonDocument get_fav_videos();
    QJsonDocument get_fav_items();
    QJsonDocument get_fav_scenes();
    QJsonDocument udc;
    QFuture<void> future;
    void get_params();
    bool upload_file(QString);
    // API
    QString api_query,fav_table;
    int check_duplicate(QString,QString,QString);
    bool check_scene_favs(QString);
    bool check_item_favs(QString);
    QByteArray readFile(QString);
    QString getFile(QString);
    bool writeFile(QString,const QByteArray);
    bool create_file(QString,const QByteArray);
    QString lat,lon;
    bool player_enabled;
    bool offline_m;
    bool recovery;
    int title_items[100],timeline_count_by_title_id[100];
    int player_offset;
    QString title_media[100];
    int title_oid[100];
    void informat(QString);
    int videos_to_unmark;
private:
    void get_remote_params();
    bool debg;
    int check_login(QString);
    QString randHash(int);
    QString logstring;
    QString randomString;
    const QString possibleCharacters="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    QFileInfo finfo;
    void get_remote_debug();
    int temp_user_no=1;
    QString get_hsh(QString,bool);
    QNetworkReply* reply;
    QString hsh;
    bool write_hsh();
    QString imgs[1000];
    int imgs_cnt;
    QList<QNetworkReply*> replies;
    int geti(QJsonDocument,QString,int i=0);
    QString gets(QJsonDocument,QString,int i=0);
    int api_ind;
signals:
    void check_carousel_displayed_fav(int,int);
    void set_access(QString);
    void next_image(int);
    void set_im(const QByteArray&);
    void set_image(int,const QByteArray);
    //    void current_user_data(QJsonObject);
    //    void set_remote_debug(QString);
    void email_already_registered();
    void inform(QString);
    void current_ver(QString);
    //   void upgrading();
    void favs_delete_done();
    void got_user_data();
    void rec_acc_not_found();
    void rec_acc_found();
    void got_favs();
    void downl_favs();
    void go_get_user_data();
    void got_titles(QJsonArray);
    //   void updater(QString,QString);
    void offline_mode(bool);
    void close_profsettings();
    void add_query(QUrl);
    void finished();

public slots:
    QPixmap circlePix(QPixmap);
    QList<QByteArray> get_reply(int);
    int make_request(QString);
    void collect_replies(QNetworkReply*);
    int pull_query(QUrl);
    void remove_from_favs(int,QString,QString);
    void get_favs(bool);
    void downloadProgress(qint64,qint64);
    void get_favs(QString,bool);
    QList<QByteArray> wait_and_get_reply(int);
    void upload_finished(QNetworkReply*);
    void get_user_data();
    void get_user_data(QString,QString,int social=0);
    void process();
    bool add_new_user();
    bool add_new_user(QString,QString,QString nick="",QString token="",int social=0);
    void add_fav_item(int,QString,QString,QString,QString,int);
    void add_fav_scene(int,QString,QString,QString,int);
    void add_fav_videos(int);

    //    int get_user(QString,QString);
    //   bool check_update();

};
void videos_added();
#endif // CREDENTIALS_H
