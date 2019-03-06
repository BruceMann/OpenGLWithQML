#include "mesh.h"


Mesh::Mesh()
{

}

Mesh::Mesh(QVector<Vertex> vert, QVector<GLuint> ind, QVector<Texture> tex):vertices(vert),indices(ind),textures(tex)
{
    this->setupMesh();
}

void Mesh::Draw(QOpenGLShaderProgram *shader)
{
    m_shader = shader;
    //bind appropriate texture
    int tmpNr = 0;
    int diffuseNr =0;
    int specularNr =0;
    for(GLuint i=0;i<this->textures.size();++i){
        glActiveTexture(GL_TEXTURE0+i);
        QString name = this->textures[i].type;
        if(name == "texture_diffuse")
            tmpNr = diffuseNr++;
        else if(name == "texture_specular")
            tmpNr = specularNr++;

        m_shader->setUniformValue(QString(name+QString::number(tmpNr++)).toStdString().c_str(),i);
        glBindTexture(GL_TEXTURE_2D,this->textures[i].id);
    }

    m_shader->setUniformValue("material.shininess",32.0f);

    //draw mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES,this->indices.size(),GL_UNSIGNED_INT,0);

    //always good practice to set everything back to defaults once configured.
    for(GLuint i=0;i<this->textures.size();++i){
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D,0);
    }
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1,&this->VAO);
    glGenBuffers(1,&this->VBO);
    glGenBuffers(1,&this->EBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER,this->VBO);
    glBufferData(GL_ARRAY_BUFFER,this->vertices.size()*sizeof(Vertex),&this->vertices[0],GL_STATIC_DRAW);

    //vertex position
    m_shader->enableAttributeArray(0);
    m_shader->setAttributeBuffer(0,GL_FLOAT,0,sizeof(Vertex));
    //vertex normal
    m_shader->enableAttributeArray(1);
    m_shader->setAttributeBuffer(1,GL_FLOAT,offsetof(Vertex,Normal),sizeof(Vertex));
    //vertex texture Coods
    m_shader->enableAttributeArray(2);
    m_shader->setAttributeBuffer(2,GL_FLOAT,offsetof(Vertex,TexCoords),sizeof(Vertex));

    glBindVertexArray(0);
}


