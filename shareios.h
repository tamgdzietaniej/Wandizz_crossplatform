#ifndef SHAREIOS_H
#define SHAREIOS_H
#include <QObject>

class shareios
{
public:
    explicit shareios();
    Q_INVOKABLE void share(const QString &text, const QUrl &url);
~shareios();
};

#endif // SHAREIOS_H
