#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QVector>

#include "mesh.h"

class Model:public QObject,protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    Model(QString path);
    void Draw(QOpenGLShaderProgram* shader);

private:

};

#endif // MODEL_H
