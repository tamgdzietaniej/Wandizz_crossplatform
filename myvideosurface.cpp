#include "myvideosurface.h"
#include <QOpenGLWidget>
#include <QVideoSurfaceFormat>
MyVideoSurface::MyVideoSurface(QObject* parent):QAbstractVideoSurface(parent)
{
    ctx = QOpenGLContext::currentContext();
    f = ctx->functions();
    active=true;
    vimage=QImage(0,0,QImage::Format_Mono);
}
void MyVideoSurface::setState(bool act){
    active=act;  
    if(!active)stop();
}
bool MyVideoSurface::present(const QVideoFrame &nframe){
    if(!active)return false;
    if ( nframe.handleType() == QAbstractVideoBuffer::GLTextureHandle ){
        if(vimage.size()!=nframe.size()){
            qDebug()<<"NEW IMAGE";
            vimage=QImage( nframe.width(), nframe.height(),QImage::Format_ARGB32);
        }
        textureId = static_cast<GLuint>(nframe.handle().toInt() );
        f->glGenFramebuffers( 1, &fbo );
        f->glGetIntegerv( GL_FRAMEBUFFER_BINDING, &prevFbo );
        f->glBindFramebuffer( GL_FRAMEBUFFER, fbo );
        f->glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0 );
        f->glReadPixels( 0, 0, nframe.width(), nframe.height(), GL_RGBA, GL_UNSIGNED_BYTE, vimage.bits() );
        f->glBindFramebuffer( GL_FRAMEBUFFER, static_cast<GLuint>( prevFbo ) );
    emit hasImage(vimage,true);
    } else {
        QVideoFrame frame(nframe);
        frame.map(QAbstractVideoBuffer::ReadOnly);
        QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(
                    frame.pixelFormat());
        vimage=QImage(frame.bits(),
                      nframe.width(),
                      nframe.height(),
                      nframe.bytesPerLine(),
                      imageFormat);
        frame.unmap();
        emit hasImage(vimage,false);
    }
    return true;
}
