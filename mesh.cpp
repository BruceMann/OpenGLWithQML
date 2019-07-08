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
    m_shader->bind();
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

    //m_shader->setUniformValue("material.shininess",32.0f);

    //draw mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES,this->indices.size(),GL_UNSIGNED_INT,0);
    glBindVertexArray(0);

    //always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    initializeOpenGLFunctions();
    glGenVertexArrays(1,&this->VAO);
    glGenBuffers(1,&this->VBO);
    glGenBuffers(1,&this->EBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER,this->VBO);
    glBufferData(GL_ARRAY_BUFFER,this->vertices.size()*sizeof(Vertex),&this->vertices[0],GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    glBindVertexArray(0);
}


