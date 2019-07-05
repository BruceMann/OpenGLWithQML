#include "myopenglwindow.h"

#include <QtQuick/qquickwindow.h>

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

#include "model.h"

Model global_Model;

MyCamera global_camera(glm::vec3(1.43, 2.6f,  2.3f));

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

//Deltatime
GLfloat deltaTime = 0.0f;   //Time between current frame and last frame
GLfloat lastFrame = 0.0f;   //Time of last frame

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

void MyOpenglWindow::sync()
{
    //qDebug()<<"void MyOpenglWindow::sync()"<<QTime::currentTime().msec();
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
    delete shader;
}

GLuint cubeVAO,cubeVBO,cubeTex;
GLuint planeVAO,planeVBO,planeTex;
void MyWindowRenderer::renderInit()
{
    qDebug()<<"void MyWindowRenderer::renderInit()";

    //    fgShaderFile = ":/shaders/base2D/base.fg";
    //    vtShaderFile = ":/shaders/base2D/base.vt";
    fgShaderFile = ":/shaders/depth_testing/depth_testing.fg";
    vtShaderFile = ":/shaders/depth_testing/depth_testing.vt";

    cubeShader = new MyShaderProgram(":/shaders/depth_testing/depth_testing.fg",":/shaders/depth_testing/depth_testing.vt");

    //cubeVAO
    glGenVertexArrays(1,&cubeVAO);
    glGenBuffers(1,&cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER,cubeVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(cubeVertices),cubeVertices,GL_STATIC_DRAW);
    cubeShader->setAttributeBuffer(0,GL_FLOAT,(0 * sizeof(GLfloat)),3,5*sizeof(GLfloat));
    cubeShader->enableAttributeArray(0);
    cubeShader->setAttributeBuffer(1,GL_FLOAT,(3 * sizeof(GLfloat)),2,5*sizeof(GLfloat));
    cubeShader->enableAttributeArray(1);
    glBindVertexArray(0);
    //plane VAO
    glGenVertexArrays(1,&planeVAO);
    glGenBuffers(1,&planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER,planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);


    genTexture(cubeTex,":/image/marble.jpg");
    genTexture(planeTex,":/image/metal.png");

    finishInit = true;


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
    tex_image = tex_image.convertToFormat(QImage::Format_RGBA8888);

    int width,height;
    imageBits =  tex_image.bits();
    width = tex_image.width();
    height = tex_image.height();

    qDebug()<<width<<height<<imageBits;

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,imageBits);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}

void MyWindowRenderer::paint()
{
    //qDebug()<<"painting"<<QTime::currentTime().msec();
    if(!finishInit){
        qDebug()<<"init error!!!";
        return;
    }
    cubeShader->bind();

    global_camera.doMovement();
    glViewport(0,0,m_viewportSize.width(), m_viewportSize.height());
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);

    cubeShader->setUniformValue("texture1",0);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = global_camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(global_camera.Zoom,(GLfloat)m_viewportSize.width()/(GLfloat)m_viewportSize.height(),0.1f,100.0f);

    cubeShader->setUniformValue("view",QMatrix4x4(glm::value_ptr(view)).transposed());
    cubeShader->setUniformValue("projection",QMatrix4x4(glm::value_ptr(projection)).transposed());

    //cube
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_2D,cubeTex);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    cubeShader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    cubeShader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    //plane
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D,planeTex);
    model = glm::mat4(1.0);
    cubeShader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

}
