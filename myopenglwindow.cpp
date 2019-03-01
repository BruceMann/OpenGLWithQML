#include "myopenglwindow.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLContext>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_0>

#include <QFile>

#include <QDebug>
#include <QImage>

#include "vertexdata.h"

#include <QMatrix4x4>
#include <QTime>

//GLM Mathmatics
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
//bool m_keys[1024] = {false};   //记录按键 实现组合按键
QMap<int,bool> m_keys;

MyOpenglWindow::MyOpenglWindow()
    :m_renderer(nullptr)
{
    setFocus(true);
    m_mixValue = 0.5;

    connect(this,&QQuickItem::windowChanged,this,&MyOpenglWindow::handleWindowChanged);

    updateTimer = new QTimer(this);
    updateTimer->setInterval(1000/60);

    connect(updateTimer,&QTimer::timeout,[=](){
        if(window()){
            window()->update();
        }
    });
    updateTimer->start();
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

void MyOpenglWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "KeyPressed:: "<<event->key();
    m_keys[event->key()] = true;
}

void MyOpenglWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat() ) {
        event->ignore();
    } else {
        qDebug() << "keyReleaseEvent:: "<<event->key() ;
        m_keys[event->key()] = false;
    }
}

void MyOpenglWindow::sync()
{
//    qDebug()<<"void MyOpenglWindow::sync()";
    if(!m_renderer){
        m_renderer = new MyWindowRenderer();
        connect(window(),&QQuickWindow::beforeRendering,m_renderer,&MyWindowRenderer::paint,Qt::DirectConnection);
    }
    m_renderer->mixValue = mixValue();
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
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    delete m_program;
}

void MyWindowRenderer::renderInit()
{
    qDebug()<<"void MyWindowRenderer::renderInit()";
    if(!m_program){
        initializeOpenGLFunctions();

        m_program = new QOpenGLShaderProgram();
        m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,vtShaderFile);
        m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,fgShaderFile);
        m_program->link();
    }

    m_program->bind();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
//    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    m_program->setAttributeBuffer(0,GL_FLOAT,(0 * sizeof(GLfloat)),3,5*sizeof(GLfloat));
    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(1,GL_FLOAT,(3 * sizeof(GLfloat)),2,5*sizeof(GLfloat));
    m_program->enableAttributeArray(1);
//    m_program->setAttributeBuffer(2,GL_FLOAT,(6 * sizeof(GLfloat)),2,8*sizeof(GLfloat));
//    m_program->enableAttributeArray(2);

    glBindVertexArray(0);

    genTexture(texture_mix,":/image/awesomeface.png");
//    genTexture(texture,":/image/wall.jpg");
    genTexture(texture,":/image/woodBox.jpg");

}

void MyWindowRenderer::genTexture(GLuint& texture,const QString& imageFile)
{

    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    QImage tex_image(imageFile);
    tex_image = tex_image.convertToFormat(QImage::Format_RGB888);

    int width,height;
    imageBits =  tex_image.bits();
    width = tex_image.width();
    height = tex_image.height();

    qDebug()<<width<<height<<imageBits;

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,imageBits);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}

void MyWindowRenderer::doMovement()
{
    //camera controll
    GLfloat cameraSpeed = 0.11f;
    if(m_keys[Qt::Key_W])
        cameraPos+=cameraSpeed*cameraFront;
    if(m_keys[Qt::Key_S])
        cameraPos-=cameraSpeed*cameraFront;
    if(m_keys[Qt::Key_A])
        cameraPos -=glm::normalize(glm::cross(cameraFront,cameraUp))*cameraSpeed;
    if(m_keys[Qt::Key_D])
        cameraPos +=glm::normalize(glm::cross(cameraFront,cameraUp))*cameraSpeed;
}

glm::vec3 cubePositions[] = {
  glm::vec3(-8.8f, -2.0f, -2.3f),
  glm::vec3( 2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3( 2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3( 1.3f, -2.0f, -2.5f),
  glm::vec3( 1.5f,  2.0f, -2.5f),
  glm::vec3( 1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
};

void MyWindowRenderer::paint()
{
    double tmp_counter = (double)timeClock.elapsed()*0.001;   //millisecond --> seconds

    doMovement();

    m_program->bind();

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

//    glDisable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture);
    m_program->setUniformValue("ourTexture1",0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,texture_mix);
    m_program->setUniformValue("ourTexture2",1);

    m_program->setUniformValue("mixValue",float(mixValue));

    //Camera/View transformation
    glm::mat4 view;
//    GLfloat radius =13.0f;
//    GLfloat camX = sin(tmp_counter)*radius;
//    GLfloat camZ = cos(tmp_counter)*radius;
//    view = glm::lookAt(glm::vec3(camX,0.0f,camZ),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
    view = glm::lookAt(cameraPos,cameraPos+cameraFront,cameraUp);

    //Projection
    glm::mat4 projection;
    projection = glm::perspective(45.0f,(GLfloat)m_viewportSize.width()/(GLfloat)m_viewportSize.height(),0.1f,100.0f);

    m_program->setUniformValue("view",QMatrix4x4(glm::value_ptr(view)).transposed());
    m_program->setUniformValue("projection",QMatrix4x4(glm::value_ptr(projection)).transposed());

    glBindVertexArray(VAO);
    for(GLuint i = 0; i < 10; i++)
    {
      glm::mat4 model;
      model = glm::translate(model, cubePositions[i]);
      GLfloat angle = 36.0f * i;
      model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
      m_program->setUniformValue("model",QMatrix4x4(glm::value_ptr(model)).transposed());

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);

//    m_program->disableAttributeArray(0);
//    m_program->disableAttributeArray(1);

//    m_program->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
//    m_window->resetOpenGLState();
}
