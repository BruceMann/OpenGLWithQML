#include "myopenglwindow.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLContext>
#include <QOpenGLExtraFunctions>

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

//    float values[] = {
//        -0.5f, -0.5f, 0.0f, // Left
//          0.5f, -0.5f, 0.0f, // Right
//          0.0f,  0.5f, 0.0f  // Top
//    };


//    GLfloat vertices[] = {
//         0.5f,  0.5f, 0.0f,  // Top Right
//         0.5f, -0.5f, 0.0f,  // Bottom Right
//        -0.5f, -0.5f, 0.0f,  // Bottom Left
//        -0.5f,  0.5f, 0.0f   // Top Left
//    };
//    GLuint indices[] = {  // Note that we start from 0!
//        0, 1, 3,  // First Triangle
//        1, 2, 3   // Second Triangle
//    };

//    GLfloat vertices[] = {
//        // First triangle
//        -0.9f, -0.5f, 0.0f,  // Left
//        -0.0f, -0.5f, 0.0f,  // Right
//        -0.45f, 0.5f, 0.0f,  // Top
//        // Second triangle
//         0.0f, -0.5f, 0.0f,  // Left
//         0.9f, -0.5f, 0.0f,  // Right
//         0.45f, 0.5f, 0.0f   // Top
//    };

GLfloat vertices[] = {
    // Positions         // Colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom Right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom Left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Top
};

void MyWindowRenderer::paint()
{
    qDebug()<<"void MyWindowRenderer::paint()";
    if(!m_program){
        initializeOpenGLFunctions();

        m_program = new QOpenGLShaderProgram();
        m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,vtShaderFile);
        m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,fgShaderFile);

//        m_program->bindAttributeLocation("vertices", 0);
//        m_program->bindAttributeLocation("color",1);
        m_program->link();
    }

    m_program->bind();

    m_program->enableAttributeArray(0);


//    GLuint EBO;
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    GLuint VBO;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    m_program->setAttributeBuffer(0,GL_FLOAT,(0 * sizeof(GLfloat)),3,24);
    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(1,GL_FLOAT,(3 * sizeof(GLfloat)),3,24);
    m_program->enableAttributeArray(1);

//    m_program->setAttributeArray(0, GL_FLOAT,vertices,3,24);

//    m_program->enableAttributeArray(0);
//    m_program->setAttributeArray(1, GL_FLOAT,vertices,3,24);

//    m_program->enableAttributeArray(1);
    //m_program->setUniformValue("ourColor",0.0,1.0,0.0,1.0);

//     GLuint VBO, VAO;

////     glGenVertexArrays(1, &VAO);
//       glGenBuffers(1, &VBO);
//       // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
////       glBindVertexArray(VAO);

//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//    // Position attribute
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
//    glEnableVertexAttribArray(0);
//    // Color attribute
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
//    glEnableVertexAttribArray(1);

////    glBindVertexArray(0);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

//    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

//    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLES, 0, 3);
//    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

//    m_program->disableAttributeArray(0);
//    m_program->disableAttributeArray(1);

    m_program->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    m_window->resetOpenGLState();

}
