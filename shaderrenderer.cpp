#include "shaderrenderer.h"

#include <QDebug>

ShaderRenderer::ShaderRenderer():
    m_shader(new MyShaderProgram())
{
    initializeOpenGLFunctions();
}

ShaderRenderer::~ShaderRenderer()
{

}

void ShaderRenderer::vertexDataParse(size_t dataSize,const float* data,int stride)
{
    qDebug()<<"void ShaderRenderer::vertexDataParse(const void* data,int stride)"<<sizeof(data)<<" "<<dataSize;

    VerticesNum = dataSize/(stride*sizeof(float));
    qDebug()<<"Vertices Number::"<<VerticesNum;

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,dataSize,data,GL_STATIC_DRAW);
    int offset = 0;
    for(int i=0;i<m_vertexInfo.size();++i){
        qDebug()<<"AAAAAAAAAA"<<i<<" "<<m_vertexInfo[i].tuple<<" "<<offset;
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i,m_vertexInfo[i].tuple,GL_FLOAT,GL_FALSE,stride*sizeof(float),(void*)(offset*sizeof(float)));
        offset+=m_vertexInfo[i].tuple;
    }
    glBindVertexArray(0);
}

void ShaderRenderer::setVertexInfo(VertexType type, int tuple)
{
    sVertexInfo info;
    info.tuple = tuple;
    info.type = type;
    m_vertexInfo.push_back(info);
}

void ShaderRenderer::setShaderProgram(QString fragmentPath, QString vertexPath, QString geometryPath)
{
    m_shader->setShaderProgram(fragmentPath,vertexPath,geometryPath);
}

MyShaderProgram *ShaderRenderer::getShaderProgram()
{
    return m_shader;
}

void ShaderRenderer::draw()
{
//    qDebug()<<"void ShaderRenderer::draw()";
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,VerticesNum);
    glBindVertexArray(0);
}
