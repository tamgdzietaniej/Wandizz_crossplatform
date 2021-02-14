#ifndef BROWSER_H
#define BROWSER_H

#include <QObject>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QWidget>
class browser : public QWidget
{
    Q_OBJECT
public:
    explicit browser(QWidget *parent = nullptr);
    QQmlContext* context;
    QQuickView* viewer;
    QQmlEngine eng;
    void loadPage(QString);
};

#endif // BROWSER_H
