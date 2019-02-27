#include "myopenglwindow.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_0>

#include <QFile>

#include <QDebug>
#include <QImage>

#include "vertexdata.h"



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

void MyWindowRenderer::genTexture(GLuint& texture,const QString& imageFile)
{

    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    QImage tex_image(imageFile);
    tex_image = tex_image.convertToFormat(QImage::Format_RGB888);

    int width,height;
    imageBits =  tex_image.bits();
    width = tex_image.width();
    height = tex_image.height();

    qDebug()<<width<<height<<imageBits;

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,imageBits);
//    glGenerateMiamap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

}

void MyWindowRenderer::paint()
{
    qDebug()<<"void MyWindowRenderer::paint()";
    if(!m_program){
        initializeOpenGLFunctions();

        m_program = new QOpenGLShaderProgram();
        m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,vtShaderFile);
        m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,fgShaderFile);
        m_program->link();
    }

    m_program->bind();

    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    m_program->setAttributeBuffer(0,GL_FLOAT,(0 * sizeof(GLfloat)),3,8*sizeof(GLfloat));
    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(1,GL_FLOAT,(3 * sizeof(GLfloat)),3,8*sizeof(GLfloat));
    m_program->enableAttributeArray(1);
    m_program->setAttributeBuffer(2,GL_FLOAT,(6 * sizeof(GLfloat)),2,8*sizeof(GLfloat));
    m_program->enableAttributeArray(2);

    genTexture(texture_mix,":/image/awesomeface.png");
    genTexture(texture,":/image/wall.jpg");

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture);
    m_program->setUniformValue("ourTexture1",0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,texture_mix);
    m_program->setUniformValue("ourTexture2",1);

    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

    m_program->disableAttributeArray(0);
    m_program->disableAttributeArray(1);
    m_program->disableAttributeArray(2);

    m_program->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    m_window->resetOpenGLState();

}
