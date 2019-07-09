#include "myshaderprogram.h"
#include <QDebug>

MyShaderProgram::MyShaderProgram()
{

}

MyShaderProgram::MyShaderProgram(QString fragmentPath, QString vertexPath,QString geometryPath)
{
    bool checkRes = true;
    checkRes &= addShaderFromSourceFile(QOpenGLShader::Vertex,vertexPath);
    checkRes &= addShaderFromSourceFile(QOpenGLShader::Fragment,fragmentPath);
    if(!geometryPath.isEmpty())
        checkRes &= addShaderFromSourceFile(QOpenGLShader::Geometry,geometryPath);
    checkRes &= link();

    if(!checkRes){
        errorMsg = log();
        qDebug()<<errorMsg;
    }
     //bind(); //编译 链接成功后手动执行绑定 glUseProgram
}

MyShaderProgram::~MyShaderProgram()
{

}
