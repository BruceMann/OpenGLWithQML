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

#include "shaderrenderer.h"

Model global_Model;

MyCamera global_camera(glm::vec3(1.0f, 1.6f,  6.3f));

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

ShaderRenderer* shaderRender = nullptr;

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
    global_camera.Pitch = -5.7f;
    global_camera.Yaw = -90.0f;

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

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,imageBits);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}

ShaderRenderer* MyWindowRenderer::getRenderer(const QString &name)
{
    ShaderRenderer* cubeRender = nullptr;
    auto iter = m_renderersMap.find(name);
    if(iter!=m_renderersMap.end())
         cubeRender = iter.value();
    if(!cubeRender){
        qDebug()<<"NULL ptr ERROR! ";
    }
    return cubeRender;
}

GLuint loadCubemap(QVector<QString>& faces){
    GLuint textureID;
    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP,textureID);
    for(GLuint i=0;i<(GLuint)(faces.size());++i){
        QImage tex_image(faces[i]);
        tex_image = tex_image.convertToFormat(QImage::Format_RGB888);
        int width,height;
        uchar* imageBits = tex_image.bits();
        if(imageBits){
            width = tex_image.width();
            height = tex_image.height();
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
                         0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,imageBits);
        }else{
            qDebug()<<"Cubemap texture failed to load at path:"<<faces[i];
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

    return textureID;
}

GLuint cubeVAO,cubeVBO,cubeTex;
GLuint planeVAO,planeVBO,planeTex;
MyShaderProgram* singleColorShader = nullptr;

GLuint quadVAO,quadVBO,quadTex;
MyShaderProgram* framebufferShader = nullptr;
GLuint framebuffer;
GLuint textureColorbuffer;
GLuint rbo;

QVector<QString> faces{
    QString(":/image/skybox/right.jpg"),
    QString(":/image/skybox/left.jpg"),
    QString(":/image/skybox/top.jpg"),
    QString(":/image/skybox/bottom.jpg"),
    QString(":/image/skybox/front.jpg"),
    QString(":/image/skybox/back.jpg"),
};
GLuint skyboxVAO,skyboxVBO,cubemapTexture;
MyShaderProgram* skyboxShader = nullptr;

MyShaderProgram* environmentMapShader = nullptr;
GLuint normalCubeVAO,normalCubeVBO;


MyShaderProgram* normal_virtualization_shader = nullptr;

MyShaderProgram* model_shader = nullptr;

MyShaderProgram* instancing_shader = nullptr;
GLuint color_quad_VAO,color_quad_VBO,instanceVBO;
glm::vec2 translations[100];
int index= 0;
float offset = 0.1f;

void MyWindowRenderer::renderInit()
{
    qDebug()<<"void MyWindowRenderer::renderInit()";

    //    fgShaderFile = ":/shaders/base2D/base.fg";
    //    vtShaderFile = ":/shaders/base2D/base.vt";
    fgShaderFile = ":/shaders/depth_testing/depth_testing.fg";
    vtShaderFile = ":/shaders/depth_testing/depth_testing.vt";

    cubeShader =        new MyShaderProgram(":/shaders/depth_testing/depth_testing.fg",
                                     ":/shaders/stencil_test/stencil_test.vt");
    singleColorShader = new MyShaderProgram(":/shaders/stencil_test/stencil_test_border.fg",
                                            ":/shaders/stencil_test/stencil_test.vt");

    framebufferShader = new MyShaderProgram(":/shaders/framebuffer/framebuffer_screen.fg",
                                            ":/shaders/framebuffer/framebuffer_screen.vt");

    skyboxShader =      new MyShaderProgram(":/shaders/skybox/skybox.fg",
                                       ":/shaders/skybox/skybox.vt");

    normal_virtualization_shader = new MyShaderProgram(":/shaders/geometry_shader/normal_visualization.fg",
                                                       ":/shaders/geometry_shader/normal_visualization.vt",
                                                       ":/shaders/geometry_shader/normal_visualization.gs");

    model_shader =      new MyShaderProgram(":/shaders/model_loading.fs",
                                       ":/shaders/model_loading.vs");

    instancing_shader = new MyShaderProgram(":/shaders/instancing/instancing.fg",
                                            ":/shaders/instancing/instancing.vt");

    //  *************test**************
    shaderRender = new ShaderRenderer();
    shaderRender->setShaderProgram(":/shaders/depth_testing/depth_testing.fg",
                                   ":/shaders/stencil_test/stencil_test.vt");
    shaderRender->setVertexInfo(VertexType::vertex_position,3);
    shaderRender->setVertexInfo(VertexType::vertex_texcoords,2);
    shaderRender->vertexDataParse(sizeof(cubeVertices),cubeVertices,5);
    //  *************test**************

    ShaderRenderer* cubeRender = new ShaderRenderer();
    cubeRender->setShaderProgram(":/shaders/depth_testing/depth_testing.fg",
                                   ":/shaders/stencil_test/stencil_test.vt");
    cubeRender->setVertexInfo(VertexType::vertex_position,3);
    cubeRender->setVertexInfo(VertexType::vertex_texcoords,2);
    cubeRender->vertexDataParse(sizeof(cubeVertices),cubeVertices,5);
    m_renderersMap.insert("cubeRender",cubeRender);

    //cubeVAO
    glGenVertexArrays(1,&cubeVAO);
    glGenBuffers(1,&cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER,cubeVBO);
    qDebug()<<"BBBBBBBBBB"<<sizeof(cubeVertices);
    glBufferData(GL_ARRAY_BUFFER,sizeof(cubeVertices),cubeVertices,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    //cube_normal VAO
    ShaderRenderer* envirCubeRender = new ShaderRenderer();
    envirCubeRender->setShaderProgram(":/shaders/environment_mapping/environment_map.fg",
                                      ":/shaders/environment_mapping/environment_map.vt");
    envirCubeRender->setVertexInfo(VertexType::vertex_position,3);
    envirCubeRender->setVertexInfo(VertexType::vertex_normal,3);
    envirCubeRender->vertexDataParse(sizeof(cubeVertices_normal),cubeVertices_normal,6);
    m_renderersMap.insert("envirCubeRender",envirCubeRender);

    //color_quad VAO
    for(int y=-10;y<10;y+=2){
        for(int x=-10;x<10;x+=2){
            glm::vec2 translation;
            translation.x = (float)x/10.0f+offset;
            translation.y = (float)y/10.0f+offset;
            translations[index++] = translation;
        }
    }
    glGenBuffers(1,&instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER,instanceVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*100,&translations[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glGenVertexArrays(1,&color_quad_VAO);
    glGenBuffers(1,&color_quad_VBO);
    glBindVertexArray(color_quad_VAO);
    glBindBuffer(GL_ARRAY_BUFFER,color_quad_VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(quadVertices_color),quadVertices_color,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER,instanceVBO);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
    glVertexAttribDivisor(2,1);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    //model data load
//    global_Model.LoadModel("C:/Users/Bruce/Documents/OpenGLWithQML/models/sphere/sphere.obj");
//     global_Model.LoadModel("D:/LearnOpenGL-master/resources/objects/rock/rock.obj");
//    global_Model.LoadModel("D:/LearnOpenGL-master/resources/objects/nanosuit/nanosuit.obj");

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

//    //skybox VAO
    glGenVertexArrays(1,&skyboxVAO);
    glGenBuffers(1,&skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER,skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glBindVertexArray(0);

//    genTexture(cubeTex,":/image/marble.jpg");
    genTexture(cubeTex,":/image/woodBox.jpg");
    genTexture(planeTex,":/image/metal.png");
    cubemapTexture = loadCubemap(faces);
    skyboxShader->bind();
    skyboxShader->setUniformValue("skybox",0);

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


    //render render into framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);

    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

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
    //write into stencil buffer
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS,1,0xFF);
    glStencilFunc(GL_KEEP,GL_KEEP,GL_REPLACE);
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D,planeTex);
    model = glm::mat4(1.0);
    cubeShader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    //cube reflection
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //enable write into color buffer
    glEnable(GL_DEPTH_TEST);                      //enable write into depth buffer
    glDepthFunc(GL_LESS);
    glStencilMask(0x00);                          //forbidden to write into stencil buffer
    glStencilFunc(GL_EQUAL,1,0xFF);
    glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
    //cube1 reflection
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,cubeTex);
    ShaderRenderer* cubeRender = getRenderer("cubeRender");
    if(!cubeRender)
        return;
    cubeRender->getShaderProgram()->bind();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    model = glm::translate(model, glm::vec3(0.0f,-0.5f,0.0f));
    model = glm::scale(model,glm::vec3(1.0f,-1.0f,1.0f));
    model = glm::translate(model, glm::vec3(0.0f,0.5f,0.0f));
    cubeRender->getShaderProgram()->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    cubeRender->getShaderProgram()->setUniformValue("view",QMatrix4x4(glm::value_ptr(view)).transposed());
    cubeRender->getShaderProgram()->setUniformValue("projection",QMatrix4x4(glm::value_ptr(projection)).transposed());
    cubeRender->draw();
    //cube 2 reflection
    shaderRender->getShaderProgram()->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,cubeTex);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f,-0.5f,0.0f));
    model = glm::scale(model,glm::vec3(1.0f,-1.0f,1.0f));
    model = glm::translate(model, glm::vec3(0.0f,0.5f,0.0f));
    shaderRender->getShaderProgram()->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    shaderRender->getShaderProgram()->setUniformValue("view",QMatrix4x4(glm::value_ptr(view)).transposed());
    shaderRender->getShaderProgram()->setUniformValue("projection",QMatrix4x4(glm::value_ptr(projection)).transposed());
    shaderRender->draw();
    glDisable(GL_STENCIL_TEST);

    // draw default cube and plane
    glBindVertexArray(planeVAO);
    cubeShader->bind();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    cubeShader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    glBindTexture(GL_TEXTURE_2D,planeTex);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_BLEND);
    glBindVertexArray(0);
//    cube1
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,cubeTex);
//    ShaderRenderer* cubeRender = getRenderer("cubeRender");
    if(!cubeRender)
        return;
    cubeRender->getShaderProgram()->bind();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    cubeRender->getShaderProgram()->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    cubeRender->draw();
    //cube2
    shaderRender->getShaderProgram()->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,cubeTex);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    shaderRender->getShaderProgram()->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    shaderRender->draw();
    glEnable(GL_STENCIL_TEST);


    //instancing quad
//    glStencilMask(0x00);
//    glBindVertexArray(color_quad_VAO);
//    instancing_shader->bind();
//    glDrawArraysInstanced(GL_TRIANGLES,0,6,100);
//    glBindVertexArray(0);

    //cube 2 use environment mapping texture
    glStencilMask(0x00);
    ShaderRenderer* envirCube = getRenderer("envirCubeRender");
    if(!envirCube)
        return;
    envirCube->getShaderProgram()->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,cubemapTexture);
    glm::vec3 cameraPos = global_camera.Position;
    envirCube->getShaderProgram()->setUniformValue("cameraPos",QVector3D(cameraPos.x,cameraPos.y,cameraPos.z));
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 2.0f));
    envirCube->getShaderProgram()->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    envirCube->getShaderProgram()->setUniformValue("view",QMatrix4x4(glm::value_ptr(view)).transposed());
    envirCube->getShaderProgram()->setUniformValue("projection",QMatrix4x4(glm::value_ptr(projection)).transposed());
    envirCube->draw();

    normal_virtualization_shader->bind();
    normal_virtualization_shader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
    normal_virtualization_shader->setUniformValue("view",QMatrix4x4(glm::value_ptr(view)).transposed());
    normal_virtualization_shader->setUniformValue("projection",QMatrix4x4(glm::value_ptr(projection)).transposed());

//    global_Model.Draw(environmentMapShader);

//    model_shader->bind();
//    model_shader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
//    model_shader->setUniformValue("view",QMatrix4x4(glm::value_ptr(view)).transposed());
//    model_shader->setUniformValue("projection",QMatrix4x4(glm::value_ptr(projection)).transposed());
//    global_Model.Draw(model_shader);  //default model
//   // global_Model.Draw(normal_virtualization_shader);   //model vertex normal visualization

    //skybox
    glStencilMask(0x00);
    glDepthFunc(GL_LEQUAL);
    skyboxShader->bind();
    glm::mat4 sky_view = glm::mat4(glm::mat3(global_camera.GetViewMatrix()));
    skyboxShader->setUniformValue("view",QMatrix4x4(glm::value_ptr(sky_view)).transposed());
    skyboxShader->setUniformValue("projection",QMatrix4x4(glm::value_ptr(projection)).transposed());
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,cubemapTexture);
    glDrawArrays(GL_TRIANGLES,0,36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    //2nd render pass
//    glStencilFunc(GL_NOTEQUAL,1,0xFF);
//    glStencilMask(0x00);
//    glDisable(GL_DEPTH_TEST);
//    GLfloat scale = 1.1f;
//    singleColorShader->bind();
//    cubeShader->bind();
////    cubeRender->getShaderProgram()->bind();
//    glBindVertexArray(cubeVAO);
//    glBindTexture(GL_TEXTURE_2D, cubeTex);
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
//    model = glm::translate(model, glm::vec3(0.0f,-0.5f,0.0f));
//    //model = glm::scale(model,glm::vec3(scale,scale,scale));
//    model = glm::scale(model,glm::vec3(1.0f,-1.0f,1.0f));
//    model = glm::translate(model, glm::vec3(0.0f,0.5f,0.0f));
////    singleColorShader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
//    cubeShader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glDrawArrays(GL_TRIANGLES, 0, 36);
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(2.0f, 0.0f, -1.0f));
//    model = glm::scale(model, glm::vec3(scale, scale, scale));
//    singleColorShader->setUniformValue("model", QMatrix4x4(glm::value_ptr(model)).transposed());
//    glDrawArrays(GL_TRIANGLES, 0, 36);
//    glBindVertexArray(0);
//    glStencilMask(0xFF);
//    glEnable(GL_DEPTH_TEST);

    //off-screen framebuffer render
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    framebufferShader->bind();
    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D,textureColorbuffer);
    glDrawArrays(GL_TRIANGLES,0,6);
}
