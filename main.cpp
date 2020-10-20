#include <navigation.h>

int main(int argc, char *argv[])
{
   qSetMessagePattern("INNNERDEBUG: %{line}-%{function}-%{message}");
#if defined(Q_OS_IOS)
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling,true);
    QApplication::setAttribute(Qt::AA_UseOpenGLES,true);
#elif defined (Q_OS_ANDROID)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling,true);
    QApplication::setAttribute(Qt::AA_UseOpenGLES,true);
   // QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

  //  QApplication::setAttribute(Qt::AA_ShareOpenGLContexts,true);
    QApplication::setAttribute(Qt::ApplicationAttribute::AA_UseHighDpiPixmaps,true);
 /*
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setSwapInterval(17);
    format.setAlphaBufferSize(8);
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CompatibilityProfile);
    format.setMajorVersion(3);
    format.setMinorVersion(0);
    format.setRenderableType(QSurfaceFormat::RenderableType::OpenGLES);
    QSurfaceFormat::setDefaultFormat(format);


   qDebug()<<"openGL ES:"<<format.majorVersion()<<format.minorVersion()<<format.renderableType()<<format<<QSurfaceFormat::defaultFormat();
   */
        #endif
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




