#ifndef MYOPENGLWINDOW_H
#define MYOPENGLWINDOW_H

#include <QObject>
#include <QQuickWindow>
#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QOpenGLExtraFunctions>

#include <QTimer>
#include <QTime>

//! [1]
class MyWindowRenderer:public QObject,protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    MyWindowRenderer():m_program(0),counter(0),loadModel_ShaderProgram(0){
        timeClock.start();
        renderInit();
    }
    ~MyWindowRenderer();

    void renderInit();

    void setViewportSize(const QSize &size){m_viewportSize = size;}
    void setWindow(QQuickWindow *window){m_window = window;}

    void genTexture(GLuint &texture, const QString &imageFile);

    GLuint texture;
    GLuint texture_mix;
    GLuint texture_specularMap;
    GLuint texture_emissionMap;
    uchar* imageBits;

    QString fgShaderFile;
    QString vtShaderFile;

    qreal mixValue;
    GLuint VAO, VBO, EBO;

    GLfloat counter;
    QTime timeClock;

//    void doMovement();   //camera control

public slots:
    void paint();

private:
    QSize m_viewportSize;
    QOpenGLShaderProgram *m_program;
    QOpenGLShaderProgram *Light_ShaderProgram;
    QOpenGLShaderProgram *loadModel_ShaderProgram;
    QQuickWindow *m_window;
};
//! [1]

//! [2]
class MyOpenglWindow:public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal mixValue READ mixValue WRITE setmixValue NOTIFY mixValueChanged)

public:
    MyOpenglWindow();

    qreal mixValue(){return m_mixValue;}
    void setmixValue(qreal value){
        m_mixValue = value;
        emit mixValueChanged(m_mixValue);
        if (window())
            window()->update();
    }

    QTimer* updateTimer;
    int updateCount;

signals:
    void mixValueChanged(qreal mixValue);


public slots:
    void sync();
    void cleaup();


private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    qreal m_mixValue;
    MyWindowRenderer* m_renderer;
};
//! [2]


#endif // MYOPENGLWINDOW_H
