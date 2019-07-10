#ifndef MESH_H
#define MESH_H

//GLM Mathmatics
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QtGui/QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>

#include <QVector>
#include <QObject>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture
{
    GLuint id;
    QString type;  //texture type::diffuse or specular
    QString path;
};

class Mesh:protected QOpenGLExtraFunctions
{
public:
    QVector<Vertex> vertices;
    QVector<GLuint> indices;
    QVector<Texture> textures;
    Mesh();
    Mesh(QVector<Vertex> vert,QVector<GLuint> ind,QVector<Texture> tex);
    void Draw(QOpenGLShaderProgram* shader);

private:
    GLuint VAO,VBO,EBO;
    QOpenGLShaderProgram* m_shader;
    void setupMesh();
};

#endif // MESH_H
