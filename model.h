#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QVector>

#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model:protected QOpenGLExtraFunctions
{
public:
    Model(QString path);
    void Draw(QOpenGLShaderProgram* shader);

private:
    QVector<Mesh> meshes;
    QString directory;
    QVector<Texture> textures_loaded;  //cache loaded texture

    void LoadModel(QString path);
    void processNode(aiNode* node,const aiScene* scene);
    Mesh processMesh(aiMesh* mesh,const aiScene* scene);
    QVector<Texture> loadMaterialTextures(aiMaterial* mat,aiTextureType type,QString typeName);
    GLuint TextureFromFile(const QString path,QString directory);
};

#endif // MODEL_H
