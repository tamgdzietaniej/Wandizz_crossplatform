#ifndef MYVIDEOSURFACE_H
#define MYVIDEOSURFACE_H
#include <QApplication>
#include <QAbstractVideoSurface>
#include <QObject>
#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QPointer>
#include <QLabel>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QImage>
class MyVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit MyVideoSurface(QObject* parent=nullptr);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const
    {
        Q_UNUSED(handleType)

        // Return the formats you will support
        return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_RGB565;
    }
      virtual ~MyVideoSurface();
    bool present(const QVideoFrame &frame);
    void setState(bool);
    bool active;
    QOpenGLContext* ctx;
    QOpenGLFunctions* f;
    GLuint textureId;
    QImage vimage;
    GLint prevFbo;
    GLuint fbo;
signals:
    void hasImage(const QImage&,bool);

};


#endif // MYVIDEOSURFACE_H
