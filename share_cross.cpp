#include "share_cross.h"

share_cross::share_cross(QObject *parent) : QObject(parent)
{

}
void share_cross::share(QString text, QUrl url)
{
    QAndroidJniObject jsText = QAndroidJniObject::fromString(text);
    QAndroidJniObject jsUrl = QAndroidJniObject::fromString(url.toString());
    QAndroidJniObject::callStaticMethod<void>("wandizz/shares/shares",
                                              "share",
                                              "(Ljava/lang/String;Ljava/lang/String;)V",
                                              jsText.object<jstring>(), jsUrl.object<jstring>());
}
