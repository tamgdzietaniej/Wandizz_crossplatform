#ifndef APPINSPECTOR_H
#define APPINSPECTOR_H
#include "globals.h"
#include <QPainter>
#include <QOpenGLWidget>
#include<QWidget>
class appinspector : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit appinspector(QWidget *parent = nullptr);
    ~appinspector() override;

    void setData(QWidget* f1,QWidget* f2);
    void addData(QList<int*>,QStringList names);
    void addData(QList<QString*>,QStringList names);
    void addData(QList<QPoint*>,QStringList names);
    void addData(QList<QSize*>,QStringList names);
    void addData(QList<QRect*>,QStringList names);
    void addData(QList<QWidget*>,QStringList names={});
    QPainter painter;
    QList<int*> integers;
    QList<QString*> strings;
    QList<QRect*> rects;
    QList<QPoint*> points;
    QList<QSize*>sizes;
    QList<QWidget*>widgets;
    void set(QString);
    void rres();
    QPoint rcol(bool i=true);
    QPoint lcol;
    int cts=0;
    int rts=0;
    QTimer t;
    QStringList intNames;
    QStringList strNames;
    QStringList rctNames;
    QStringList pntNames;
    QStringList sizNames;
    QStringList widNames;
protected:
    void paintGL() override;
    void initializeGL() override;
};

#endif // APPINSPECTOR_H
