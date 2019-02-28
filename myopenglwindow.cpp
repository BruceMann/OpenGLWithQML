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

//GLM Mathmatics
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


MyOpenglWindow::MyOpenglWindow()
    :m_renderer(nullptr)
{
    m_mixValue = 0.5;
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
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

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

    glBindVertexArray(0);

    genTexture(texture_mix,":/image/awesomeface.png");
    genTexture(texture,":/image/wall.jpg");
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

void MyWindowRenderer::paint()
{
    qDebug()<<"void MyWindowRenderer::paint()";
    m_program->bind();

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

//    glDisable(GL_DEPTH_TEST);

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

    m_program->setUniformValue("mixValue",float(mixValue));

//   QTransform translationTransform;
//   translationTransform.translate(0.1,0.1);
//   translationTransform.rotate(45,Qt::YAxis);
//   translationTransform.rotate(45,Qt::XAxis);
//   translationTransform.rotate(45,Qt::ZAxis);
//   translationTransform.scale(1.5,1.5);
    //m_program->setUniformValue("transform",translationTransform.toAffine());

//    float aaa[16]={0.0f};
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    model = glm::rotate(model,-45.0f,glm::vec3(1.0f,0.0f,0.0f));
    model = glm::scale(model,glm::vec3(2.0f,2.0f,1.0f));
//    model =glm::translate(model,glm::vec3(0.3f,0.0f,0.0f));
    view = glm::translate(view,glm::vec3(0.0f,0.0f,-3.0f));
    projection = glm::perspective(45.0f,(GLfloat)m_viewportSize.width()/(GLfloat)m_viewportSize.height(),0.1f,100.0f);

    QMatrix4x4 m1 = QMatrix4x4(glm::value_ptr(model));
    qDebug()<<m1;
    qDebug()<<m1.transposed();


    m_program->setUniformValue("model",QMatrix4x4(glm::value_ptr(model)).transposed());
    m_program->setUniformValue("view",QMatrix4x4(glm::value_ptr(view)).transposed());
    m_program->setUniformValue("projection",QMatrix4x4(glm::value_ptr(projection)).transposed());

//    // Create transformations
//    glm::mat4 model;
//    glm::mat4 view;
//    glm::mat4 projection;
//    model = glm::rotate(model, -45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
////    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
//    projection = glm::perspective(45.0f, (GLfloat)600 / (GLfloat)600, 0.1f, 100.0f);

//    float dArray[16] = {0.0f};
//    const float *p = (const float*)glm::value_ptr(view);
//    for(int i = 0;i<16;++i){
//        dArray[i] = p[i];
////        std::cout<<dArray[i];
//    }

//    QMatrix4x4 m1 = QMatrix4x4(dArray);
//    qDebug()<<m1;


//    GLint modelLoc = glGetUniformLocation(m_program->programId(), "model");
//    GLint viewLoc = glGetUniformLocation(m_program->programId(), "view");
//    GLint projLoc = glGetUniformLocation(m_program->programId(), "projection");
//    // Pass them to the shaders
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
//    // Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
//    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));



    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
    glBindVertexArray(0);

//    m_program->disableAttributeArray(0);
//    m_program->disableAttributeArray(1);
//    m_program->disableAttributeArray(2);

//    m_program->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    m_window->resetOpenGLState();

}
