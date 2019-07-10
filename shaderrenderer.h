//2019.7.10  统一处理顶点，纹理

#ifndef SHADERRENDERER_H
#define SHADERRENDERER_H

#include <QObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>
#include <QVector2D>
#include <QVector3D>

#include "myshaderprogram.h"

struct sVertex
{
    QVector3D position;
    QVector3D normal;
    QVector2D texCoords;
//    QVector3D Tangent;
//    QVector3D Bitangent;
};

struct sTexture
{
    unsigned int id;
    QString type;
    QString path;
};

enum VertexType{
    vertex_position,
    vertex_normal,
    vertex_texcoords
};

struct sVertexInfo{
    VertexType type;
    int        tuple;
    //sVertexInfo(VertexType _type,int _tuple){type=_type;tuple=_tuple;}
};

class ShaderRenderer:public QObject,public QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    ShaderRenderer();
    ~ShaderRenderer();

public:
    QVector<sVertex>        m_vectices;
    QVector<sTexture>       m_textures;
    QVector<GLuint>         m_indices;
    QVector<sVertexInfo>    m_vertexInfo;


    void vertexDataParse(size_t dataSize,const float *data, int stride);

    void setVertexInfo(VertexType type,int tuple);
    void setShaderProgram(QString fragmentPath,QString vertexPath,QString geometryPath = QString());

    MyShaderProgram* getShaderProgram();

    void draw();

private:
    GLuint                   VAO,VBO,EBO;
    int                     VerticesNum;
    MyShaderProgram*        m_shader;

};

#endif // SHADERRENDERER_H
