#ifndef MYOPENGLWINDOW_H
#define MYOPENGLWINDOW_H

#include <QObject>
#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>

//! [1]
class MyWindowRenderer:public QObject,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    MyWindowRenderer():m_program(0){
        // test password change  2019/2/27
        fgShaderFile = ":/shaders/fragment_shader.frg";
        vtShaderFile = ":/shaders/vertex_shader.vtx";

    }
    ~MyWindowRenderer();

    void setViewportSize(const QSize &size){m_viewportSize = size;}
    void setWindow(QQuickWindow *window){m_window = window;}

    void genTexture(GLuint &texture, const QString &imageFile);
    GLuint texture;
    GLuint texture_mix;
    uchar* imageBits;

    QString fgShaderFile;
    QString vtShaderFile;


public slots:
    void paint();

private:
    QSize m_viewportSize;
    QOpenGLShaderProgram *m_program;
    QQuickWindow *m_window;

};
//! [1]

//! [2]
class MyOpenglWindow:public QQuickItem
{
    Q_OBJECT
public:
    MyOpenglWindow();

    void readShaderFile(QString vxShaderFile, QString fgShaderFile);





public slots:
    void sync();
    void cleaup();


private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    MyWindowRenderer* m_renderer;


};
//! [2]

#endif // MYOPENGLWINDOW_H
