#ifndef MESH_H
#define MESH_H

//GLM Mathmatics
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>

#include <QVector>
#include <QObject>

struct Vertex
{
    glm::vec3 Position;
    glm::vec2 Normal;
    glm::vec2 TexCoords;
};

struct Texture
{
    GLuint id;
    QString type;  //texture type::diffuse or specular
};

class Mesh:public QObject,protected QOpenGLExtraFunctions
{
    Q_OBJECT
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
