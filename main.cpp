#include <navigation.h>
#include <QApplication>
int main(int argc, char *argv[])
{
   // qSetMessagePattern("INNERDEBUG: %{line}-%{function}-%{message}");
#if defined(Q_OS_IOS)
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling,true);
 #elif defined (Q_OS_ANDROID)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling,true);
  #endif
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer);
    format.setSwapInterval(1);
    format.setAlphaBufferSize(8);
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
    format.setMajorVersion(2);
    format.setMinorVersion(0);
    format.setRenderableType(QSurfaceFormat::RenderableType::OpenGLES);
    QSurfaceFormat::setDefaultFormat(format);
        QApplication::setAttribute(Qt::AA_ShareOpenGLContexts,true);
    QApplication::setAttribute(Qt::AA_UseOpenGLES,true);
    QApplication::setAttribute(Qt::ApplicationAttribute::AA_UseHighDpiPixmaps,true);
    QApplication app(argc, argv);
    //  QApplication::setAttribute(Qt::AA_DontCheckOpenGLContextThreadAffinity);
    // QApplication::setApplicationVersion("6.0.0.0");
    // QApplication::setApplicationName("WANDIZZ");
    //   QApplication::setApplicationDisplayName("WANDIZZ");

    navigation controller;
#if defined (Q_OS_MACX)
    controller.show();
#else
    controller.showFullScreen();
#endif
    app.exec();
}




