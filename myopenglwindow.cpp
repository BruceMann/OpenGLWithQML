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



GLuint cubeVAO,cubeVBO,cubeTex;
GLuint planeVAO,planeVBO,planeTex;
MyShaderProgram* singleColorShader = nullptr;

GLuint quadVAO,quadVBO,quadTex;
MyShaderProgram* framebufferShader = nullptr;
GLuint framebuffer;
GLuint textureColorbuffer;
 GLuint rbo;
void MyWindowRenderer::renderInit()
{
    qDebug()<<"void MyWindowRenderer::renderInit()";

    //    fgShaderFile = ":/shaders/base2D/base.fg";
    //    vtShaderFile = ":/shaders/base2D/base.vt";
    fgShaderFile = ":/shaders/depth_testing/depth_testing.fg";
    vtShaderFile = ":/shaders/depth_testing/depth_testing.vt";

    cubeShader = new MyShaderProgram(":/shaders/depth_testing/depth_testing.fg",":/shaders/stencil_test/stencil_test.vt");
    singleColorShader = new MyShaderProgram(":/shaders/stencil_test/stencil_test_border.fg",":/shaders/stencil_test/stencil_test.vt");
    framebufferShader = new MyShaderProgram(":/shaders/framebuffer/framebuffer_screen.fg",":/shaders/framebuffer/framebuffer_screen.vt");

    //cubeVAO
    glGenVertexArrays(1,&cubeVAO);
    glGenBuffers(1,&cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER,cubeVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(cubeVertices),cubeVertices,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    //plane VAO
    glGenVertexArrays(1,&planeVAO);
    glGenBuffers(1,&planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer (GL_ARRAY_BUFFER,planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    //screen quad VAO
    glGenVertexArrays(1,&quadVAO);
    glGenBuffers(1,&quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(quadVertices),&quadVertices,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));

    genTexture(cubeTex,":/image/marble.jpg");
    genTexture(planeTex,":/image/metal.png");

    framebufferShader->bind();
    framebufferShader->setUniformValue("screenTexture",0);
    //framebuffer configuration

    glGenFramebuffers(1,&framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
    //create a color attachment texture

    glGenTextures(1,&textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D,textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,800,800,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textureColorbuffer,0);
    //create a renderbuffer object for depth and stencil attachment

    glGenRenderbuffers(1,&rbo);
    glBindRenderbuffer(GL_RENDERBUFFER,rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,800,800);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,rbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
        qDebug()<<"error :: framebuffer is not complete!";
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    finishInit = true;
}

void MyWindowRenderer::paint()
{
    //qDebug()<<"painting"<<QTime::currentTime().msec();
    if(!finishInit){
        qDebug()<<"init error!!!";
        return;
    }
    global_camera.doMovement();
    glViewport(0,0,m_viewportSize.width(), m_viewportSize.height());

    //render
    glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = global_camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(global_camera.Zoom,(GLfloat)m_viewportSize.width()/(GLfloat)m_viewportSize.height(),0.1f,100.0f);


    singleColorShader->bind();
    singleColorShader->setUniformValue("view",QMatrix4x4(glm::value_ptr(view)).transposed());
    singleColorShader->setUniformValue("projection",QMatrix4x4(glm::value_ptr(projection)).transposed());

    cubeShader->bind();
    cubeShader->setUniformValue("texture1",0);
    cubeShader->setUniformValue("view",QMatrix4x4(glm::value_ptr(view)).transposed());
    cubeShader->setUniformValue("projection",QMatrix4x4(glm::value_ptr(projection)).transposed());

    //plane
    glStencilMask(0x00);
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D,planeTex);
    model = glm::mat4(1.0);
    cubeShader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    //cube
    glStencilFunc(GL_ALWAYS,1,0xFF);
    glStencilMask(0xFF); //enable write to the stencil buffer

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,cubeTex);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    cubeShader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    cubeShader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    //2nd render pass
    glStencilFunc(GL_NOTEQUAL,1,0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    GLfloat scale = 1.1f;
    singleColorShader->bind();
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_2D, cubeTex);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    model = glm::scale(model,glm::vec3(scale,scale,scale));
    singleColorShader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    singleColorShader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    framebufferShader->bind();
    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D,textureColorbuffer);
    glDrawArrays(GL_TRIANGLES,0,6);
}
