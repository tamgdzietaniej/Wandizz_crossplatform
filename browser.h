#ifndef BROWSER_H
#define BROWSER_H

#include <QObject>
#include <QQuickWidget>
#include <QQmlContext>
#include <QQmlEngine>
class browser : public QQuickWidget
{
    Q_OBJECT
public:
    explicit browser(QWidget *parent = nullptr);
    QQmlContext* context;
    void loadPage(QString);
};

#endif // BROWSER_H
