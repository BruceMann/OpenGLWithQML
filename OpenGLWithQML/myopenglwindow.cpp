#include "myopenglwindow.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLContext>

#include <QFile>

#include <QDebug>

MyOpenglWindow::MyOpenglWindow()
    :m_renderer(nullptr)
{
    connect(this,&QQuickItem::windowChanged,this,&MyOpenglWindow::handleWindowChanged);

}

void MyOpenglWindow::readShaderFile( QString vxShaderFile,  QString fgShaderFile)
{
    QFile* vsFile;
    QFile* fgFile;
    qDebug()<<vxShaderFile;
    vsFile = new QFile(vxShaderFile);
    if(vsFile->exists()){
        if(vsFile->open(QIODevice::ReadOnly)){
            //m_renderer->m_vt_shader_file = QString(vsFile->readAll());
            qDebug()<<QString(vsFile->readAll());
        }else
            qDebug()<<"vxShaderFile open failed !!!";
    }else
        qDebug()<<"vxShaderFile is not existed !!!";

    qDebug()<<fgShaderFile;
    fgFile = new QFile(fgShaderFile);
    if(fgFile->exists()){
        if(fgFile->open(QIODevice::ReadOnly)){
            //m_renderer->m_fg_shader_file = QString(fgFile->readAll());
            qDebug()<<QString(fgFile->readAll());
        }else
            qDebug()<<"vxShaderFile open failed !!!";
    }else
        qDebug()<<"vxShaderFile is not existed !!!";


//    m_renderer->m_vt_shader_file = vxShaderFile;
//    m_renderer->m_fg_shader_file = fgShaderFile;

    //return true;
}

void MyOpenglWindow::sync()
{
    qDebug()<<"void MyOpenglWindow::sync()";
    if(!m_renderer){
        m_renderer = new MyWindowRenderer();
        connect(window(),&QQuickWindow::beforeRendering,m_renderer,&MyWindowRenderer::paint,Qt::DirectConnection);
    }
    m_renderer->setViewportSize(window()->size()*window()->devicePixelRatio());
    m_renderer->setWindow(window());
}

void MyOpenglWindow::cleaup()
{
    qDebug()<<"void MyOpenglWindow::cleaup()";
    if(m_renderer){
        delete m_renderer;
        m_renderer = nullptr;
    }
}

void MyOpenglWindow::handleWindowChanged(QQuickWindow *win)
{
    qDebug()<<"void MyOpenglWindow::handleWindowChanged(QQuickWindow *win)";
    if(win){
        connect(win,&QQuickWindow::beforeSynchronizing,this,&MyOpenglWindow::sync,Qt::DirectConnection);
        connect(win,&QQuickWindow::sceneGraphInvalidated,this,&MyOpenglWindow::cleaup,Qt::DirectConnection);

        win->setClearBeforeRendering(false);
    }
}

MyWindowRenderer::~MyWindowRenderer()
{
    delete m_program;
}




void MyWindowRenderer::paint()
{
    qDebug()<<"void MyWindowRenderer::paint()";
    if(!m_program){
        initializeOpenGLFunctions();

        m_program = new QOpenGLShaderProgram();
//        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,
//                                                    "attribute highp vec4 vertices;"
//                                                    "varying highp vec2 coords;"
//                                                    "void main() {"
//                                                    "    gl_Position = vertices;"
//                                                    "    coords = vertices.xy;"
//                                                    "}");
//        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,
//                                                    "uniform lowp float t;"
//                                                    "varying highp vec2 coords;"
//                                                    "void main() {"
//                                                    "    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));"
//                                                    "    i = smoothstep(t - 0.8, t + 0.8, i);"
//                                                    "    i = floor(i * 20.) / 20.;"
//                                                    "    gl_FragColor = vec4(coords.x,.0,.0,1.0);"
//                                                    "}");
        m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,vtShaderFile);
        m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,fgShaderFile);

        m_program->bindAttributeLocation("vertices", 0);
        m_program->link();
    }

    m_program->bind();

    m_program->enableAttributeArray(0);

    float values[] = {
        -1, -1,
        1, -1,
        -1, 1,
        1, 1
    };
    m_program->setAttributeArray(0, GL_FLOAT, values, 2);
//    m_program->setUniformValue("t", (float) m_t);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program->disableAttributeArray(0);
    m_program->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    m_window->resetOpenGLState();

}
