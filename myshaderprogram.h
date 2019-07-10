#ifndef MYSHADERPROGRAM_H
#define MYSHADERPROGRAM_H

#include <QOpenGLShaderProgram>

class MyShaderProgram:public QOpenGLShaderProgram
{
public:
    MyShaderProgram();
    MyShaderProgram(QString fragmentPath,QString vertexPath,QString geometryPath = QString());
    ~MyShaderProgram();

    void setShaderProgram(QString fragmentPath,QString vertexPath,QString geometryPath = QString());

    QString errorMsg;
};

#endif // MYSHADERPROGRAM_H
