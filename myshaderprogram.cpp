#include "myshaderprogram.h"
#include <QDebug>

MyShaderProgram::MyShaderProgram()
{

}

MyShaderProgram::MyShaderProgram(QString fragmentPath, QString vertexPath)
{
    bool checkRes = true;
    checkRes &= addShaderFromSourceFile(QOpenGLShader::Vertex,vertexPath);
    checkRes &= addShaderFromSourceFile(QOpenGLShader::Fragment,fragmentPath);
    checkRes &= link();

    if(!checkRes){
        errorMsg = log();
        qDebug()<<errorMsg;
    }
     bind(); //编译 链接成功后手动执行绑定 glUseProgram
}

MyShaderProgram::~MyShaderProgram()
{

}
