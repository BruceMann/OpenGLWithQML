#include "model.h"

#include <QImage>

#include <QDebug>

Model::Model()
{

}

Model::Model(QString path)
{
    this->LoadModel(path);
}

void Model::Draw(QOpenGLShaderProgram *shader)
{
    for(GLuint i=0;i<this->meshes.size();++i){
        this->meshes[i].Draw(shader);
    }
}

void Model::LoadModel(QString path)
{
    initializeOpenGLFunctions();
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.toStdString(),aiProcess_Triangulate|aiProcess_FlipUVs);
    //check for errors
    if(!scene||scene->mFlags==AI_SCENE_FLAGS_INCOMPLETE||!scene->mRootNode)
    {
        qDebug()<<"ERROR:: ASSIMP:: "<<importer.GetErrorString();
        return;
    }
    //retrieve the directory path of the filepath
    this->directory = path.mid(0,path.lastIndexOf('/'));
    qDebug()<<"LoadModel:: "<<path<<" directory:: "<<directory;

    this->processNode(scene->mRootNode,scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for(GLuint i=0;i<node->mNumMeshes;++i){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.append(this->processMesh(mesh,scene));
    }
    // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(GLuint i = 0; i < node->mNumChildren; i++)
    {
        this->processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    //Data to fill
    QVector<Vertex> vertices;
    QVector<GLuint> indices;
    QVector<Texture> textures;

    //walk through each of the mesh's vertices
    for(GLuint i = 0;i<mesh->mNumVertices;++i){
        Vertex vertex;
        //position
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        //normal
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
        //Texture Coordinates
        if(mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }else
            vertex.TexCoords = glm::vec2(0.0f,0.0f);
        vertices.append(vertex);
    }

    //indices
    for(GLuint i=0;i<mesh->mNumFaces;++i){
        aiFace face = mesh->mFaces[i];
        for(GLuint j=0;j<face.mNumIndices;j++)
            indices.push_back(face.mIndices[j]);
    }

    //material
    if(mesh->mMaterialIndex>=0){
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
        // Same applies to other texture as the following list summarizes:
        // Diffuse: texture_diffuseN
        // Specular: texture_specularN
        // Normal: texture_normalN

        //1.Diffuse maps
        QVector<Texture> diffuseMaps = this->loadMaterialTextures(material,aiTextureType_DIFFUSE,"texture_diffuse");
        textures+=diffuseMaps;
        //2.Specular maps
        QVector<Texture> specularMaps = this->loadMaterialTextures(material,aiTextureType_SPECULAR,"texture_specular");
        textures+=specularMaps;
    }

    return Mesh(vertices,indices,textures);
}

QVector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, QString typeName)
{
    QVector<Texture> textures;
    for(GLuint i=0;i<mat->GetTextureCount(type);++i){
        aiString str;
        mat->GetTexture(type,i,&str);
        std::string sttt(str.C_Str());
        QString qStr = QString::fromStdString(sttt);
        qDebug()<<qStr;
        bool isloaded = false;
        for(GLuint j;j<textures_loaded.size();++j){
            if(textures_loaded[i].path==qStr){
                isloaded = true;
                textures.append(textures_loaded[j]);
                break;
            }
        }
        if(!isloaded){
            Texture texture;
            texture.id = TextureFromFile(qStr,this->directory);
            texture.type = typeName;
            texture.path = qStr;
            textures.append(texture);
        }
    }
    return textures;
}

GLuint Model::TextureFromFile(const QString path, QString directory)
{
    //Generate texture ID and load texture data
    QString filename = QString(path);
    filename = directory+"/"+filename;
    qDebug()<<filename;
    GLuint textureID;
    glGenTextures(1,&textureID);
    int width,height;
    uchar* image_bits;
    QImage image(filename);
    image = image.convertToFormat(QImage::Format_RGB888);
    width = image.width();
    height = image.height();
    image_bits = image.bits();

    //assign texture to ID
    glBindTexture(GL_TEXTURE_2D,textureID);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,(GLvoid*)image_bits);
    glGenerateMipmap(GL_TEXTURE_2D);

    //parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);
    return textureID;
}
