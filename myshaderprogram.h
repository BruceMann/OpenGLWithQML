#ifndef MYSHADERPROGRAM_H
#define MYSHADERPROGRAM_H

#include <QOpenGLShaderProgram>

class MyShaderProgram:public QOpenGLShaderProgram
{
public:
    MyShaderProgram();
    MyShaderProgram(QString fragmentPath,QString vertexPath);
    ~MyShaderProgram();

    QString errorMsg;
};

#endif // MYSHADERPROGRAM_H