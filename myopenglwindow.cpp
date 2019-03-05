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

#include "mycamera.h"

extern MyCamera global_camera(glm::vec3(1.43, 2.6f,  2.3f));

//glm::vec3 lightPos(-3.0f,5.0f, -5.0f);
glm::vec3 lightPos(0.0f,0.0f,0.0f);

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

// Camera
//glm::vec3 cameraPos   = glm::vec3(1.0f, 2.0f,  5.0f);
//glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
//GLfloat yaw = -90.0f;
//GLfloat pitch = -10.0f;
//GLfloat lastX = 300.0f;
//GLfloat lastY = 300.0f;
//GLfloat fov = 45.0f;

//Deltatime
GLfloat deltaTime = 0.0f;   //Time between current frame and last frame
GLfloat lastFrame = 0.0f;   //Time of last frame

//bool m_keys[1024] = {false};   //记录按键 实现组合按键
QMap<int,bool> m_keys;

MyOpenglWindow::MyOpenglWindow()
    :m_renderer(nullptr)
{
    this->setObjectName("MyOpenglWindow");
    setFocus(true);
    setAcceptHoverEvents(true);
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

    global_camera.MovementSpeed=0.08f;
    global_camera.Pitch = -35.7f;
    global_camera.Yaw = -111.3f;

    this->installEventFilter(&global_camera);

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

//void MyOpenglWindow::keyPressEvent(QKeyEvent *event)
//{
////    qDebug() << "KeyPressed:: "<<event->key();
//    m_keys[event->key()] = true;
//}

//void MyOpenglWindow::keyReleaseEvent(QKeyEvent *event)
//{
//    if(event->isAutoRepeat() ) {
//        event->ignore();
//    } else {
////        qDebug() << "keyReleaseEvent:: "<<event->key() ;
//        m_keys[event->key()] = false;
//    }
//}

//void MyOpenglWindow::hoverEnterEvent(QHoverEvent *event)
//{
//    lastX = (GLfloat)event->posF().x();
//    lastY = (GLfloat)event->posF().y();

//    qDebug()<<"hoverEnter event:: "<<lastX<<" "<<lastY;
//}

//void MyOpenglWindow::hoverMoveEvent(QHoverEvent *event)
//{
//    GLfloat curPosX = (GLfloat)event->posF().x();
//    GLfloat curPosY = (GLfloat)event->posF().y();
//    qDebug()<<"hoverMoveEvent event:: "<<curPosX<<" "<<curPosY;

//    GLfloat xoffset = curPosX - lastX;
//    GLfloat yoffset = lastY - curPosY;
//    lastX = curPosX;
//    lastY = curPosY;
//    qDebug()<<"xoffset::  "<<xoffset<<"yoffset:: "<<yoffset;

//    GLfloat sensitivity = 0.1f;
//    xoffset *= sensitivity;
//    yoffset *= sensitivity;

//    yaw += xoffset;
//    pitch += yoffset;

//    if(pitch>89.0f)
//        pitch = 89.0f;
//    if(pitch<-89.0f)
//        pitch = -89.0f;

//    glm::vec3 front;
//    front.x = cos(glm::radians(yaw))*cos(glm::radians(pitch));
//    front.y = sin(glm::radians(pitch));
//    front.z = sin(glm::radians(yaw))*cos(glm::radians(pitch));
//    cameraFront = glm::normalize(front);
//}

//void MyOpenglWindow::hoverLeaveEvent(QHoverEvent *event)
//{
//    Q_UNUSED(event)

//}

//void MyOpenglWindow::wheelEvent(QWheelEvent *event)
//{
//    qDebug()<<"wheelEvent ::  "<<event->angleDelta().x()<<" "<<event->angleDelta().y();
//    QPoint numSteps = event->angleDelta()/120;
//    qDebug()<<"wheelEvent ::  "<<numSteps.x()<<" "<<numSteps.y();
//    double yoffset = numSteps.y();

//    if(fov>=1.0f && fov <=45.0f)
//        fov -= yoffset*0.1;
//    if (fov <= 1.0f)
//        fov = 1.0f;
//    if (fov >= 45.0f)
//        fov = 45.0f;

//    qDebug()<<"FOV"<<fov;

//}

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

    fgShaderFile = ":/shaders/fragment_shader.frg";
    vtShaderFile = ":/shaders/vertex_shader.vtx";

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

    m_program->setAttributeBuffer(0,GL_FLOAT,(0 * sizeof(GLfloat)),3,8*sizeof(GLfloat));
    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(1,GL_FLOAT,(3 * sizeof(GLfloat)),3,8*sizeof(GLfloat));
    m_program->enableAttributeArray(1);
    m_program->setAttributeBuffer(2,GL_FLOAT,(6 * sizeof(GLfloat)),2,8*sizeof(GLfloat));
    m_program->enableAttributeArray(2);

    glBindVertexArray(0);

//    genTexture(texture_mix,":/image/awesomeface.png");
//    genTexture(texture,":/image/wall.jpg");
//    genTexture(texture,":/image/woodBox.jpg");
    genTexture(texture,":/image/container2.png");
    genTexture(texture_specularMap,":/image/container2_specular.png");
    genTexture(texture_emissionMap,":/image/matrix.jpg");


//    if(!Light_ShaderProgram){
//        initializeOpenGLFunctions();
//        Light_ShaderProgram = new QOpenGLShaderProgram();
//        Light_ShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/color_light.vtx");
//        Light_ShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/color_light.frg");
//        Light_ShaderProgram->link();
//    }


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

//void MyWindowRenderer::doMovement()
//{
//    //camera controll
//    GLfloat cameraSpeed = 0.11f;
//    if(m_keys[Qt::Key_W])
//        cameraPos+=cameraSpeed*cameraFront;
//    if(m_keys[Qt::Key_S])
//        cameraPos-=cameraSpeed*cameraFront;
//    if(m_keys[Qt::Key_A])
//        cameraPos -=glm::normalize(glm::cross(cameraFront,cameraUp))*cameraSpeed;
//    if(m_keys[Qt::Key_D])
//        cameraPos +=glm::normalize(glm::cross(cameraFront,cameraUp))*cameraSpeed;
//}

void MyWindowRenderer::paint()
{
    double tmp_counter = (double)timeClock.elapsed()*0.001;   //millisecond --> seconds

//    doMovement();
    global_camera.doMovement();

    m_program->bind();

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

//    glDisable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);

//    glClearColor(0.1f, 0.21f, 0.16f, 1.0f);
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture);
    m_program->setUniformValue("material.diffuse",0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,texture_specularMap);
    m_program->setUniformValue("material.specular",1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,texture_emissionMap);
    m_program->setUniformValue("material.emission",2);

//    m_program->setUniformValue("ourTexture1",0);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D,texture_mix);
//    m_program->setUniformValue("ourTexture2",1);

//    m_program->setUniformValue("mixValue",float(mixValue));


    m_program->setUniformValue("objectColor",1.0f,0.5f,0.31f);
    m_program->setUniformValue("lightColor",1.0f,1.0f,1.0f);
    //m_program->setUniformValue("lightPos",lightPos.x,lightPos.y,lightPos.z);
    m_program->setUniformValue("viewPos",global_camera.Position.x,global_camera.Position.y,global_camera.Position.z);

    // set light
    glm::vec3 lightColor;
    lightColor.x = sin(tmp_counter * 2.0f);
    lightColor.y = sin(tmp_counter * 0.7f);
    lightColor.z = sin(tmp_counter* 1.3f);

    glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

    //m_program->setUniformValue("light.position",lightPos.x,lightPos.y,lightPos.z);
    m_program->setUniformValue("light.direction",-0.2f,-1.0f,-0.3f);
//    m_program->setUniformValue("light.ambient",ambientColor.x,ambientColor.y,ambientColor.z);
//    m_program->setUniformValue("light.diffuse",diffuseColor.x,diffuseColor.y,diffuseColor.z);
    m_program->setUniformValue("light.specular",1.0f,1.0f,1.0f);
    m_program->setUniformValue("light.ambient",0.2f,0.2f,0.2f);
    m_program->setUniformValue("light.diffuse",0.5f,0.5f,0.5f);
//    m_program->setUniformValue("light.specular",1.0f,1.0f,1.0f);

    // 设置点光源衰减系数
//    m_program->setUniformValue("light.constant",1.0f);
//    m_program->setUniformValue("light.linear",0.09f);
//    m_program->setUniformValue("light.quadratic",0.032f);

    //设置聚光参数
    m_program->setUniformValue("light.direction",global_camera.Front.x,global_camera.Front.y,global_camera.Front.z);
    m_program->setUniformValue("light.position",global_camera.Position.x,global_camera.Position.y,global_camera.Position.z);
    m_program->setUniformValue("light.cutOff",glm::cos(glm::radians(12.5f)));
    m_program->setUniformValue("light.outCutOff",glm::cos(glm::radians(15.5f))); //又打错了 吐血啊 outCurOff-->outCutOff;

    // set material
    m_program->setUniformValue("material.ambient",1.0f,0.5f,0.31f);
//    m_program->setUniformValue("material.diffuse",1.0f,0.5f,0.31f);
//    m_program->setUniformValue("material.specular",0.5f,0.5f,0.5f);
    m_program->setUniformValue("material.shininess",32.0f);  //打错material了 我有毒-，-
    //青色(Cyan)的塑料箱子
//    m_program->setUniformValue("material.ambient",0.0f, 0.1f, 0.06f);
//    m_program->setUniformValue("material.diffuse", 0.0f, 0.50980392f, 0.50980392f);
//    m_program->setUniformValue("material.specular",0.50196078f, 0.50196078f, 0.50196078f);

    //Camera/View transformation
    glm::mat4 view;
//    GLfloat radius =13.0f;
//    GLfloat camX = sin(tmp_counter)*radius;
//    GLfloat camZ = cos(tmp_counter)*radius;
//    view = glm::lookAt(glm::vec3(camX,0.0f,camZ),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
//    view = glm::lookAt(cameraPos,cameraPos+cameraFront,cameraUp);
    view = global_camera.GetViewMatrix();

    //Projection
    glm::mat4 projection;
    projection = glm::perspective(global_camera.Zoom,(GLfloat)m_viewportSize.width()/(GLfloat)m_viewportSize.height(),0.1f,100.0f);

    m_program->setUniformValue("view",QMatrix4x4(glm::value_ptr(view)).transposed());
    m_program->setUniformValue("projection",QMatrix4x4(glm::value_ptr(projection)).transposed());


    glBindVertexArray(VAO);
    for(GLuint i = 0; i < 10; i++)
    {
      glm::mat4 model;
      model = glm::translate(model, cubePositions[i]);
      GLfloat angle = 20.0f * i ;
      model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
      m_program->setUniformValue("model",QMatrix4x4(glm::value_ptr(model)).transposed());

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
//    glm::mat4 model;
//    model = glm::translate(model,glm::vec3(.0f,.0f,.0f));
//    m_program->setUniformValue("model",QMatrix4x4(glm::value_ptr(model)).transposed());
//    glDrawArrays(GL_TRIANGLES,0,36);

    glBindVertexArray(0);







//    m_program->disableAttributeArray(0);
//    m_program->disableAttributeArray(1);

//    m_program->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
//    m_window->resetOpenGLState();
}
