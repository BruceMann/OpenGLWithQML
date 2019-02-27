#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "myopenglwindow.h"

#include <QDir>
#include <QDebug>
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qDebug()<<QDir::currentPath();

//    MyOpenglWindow* myWindow = new MyOpenglWindow;
//    QString fgShaderFile = "G:\\OpenGLWithQML\\OpenGLWithQML\\shaders\\fragment_shader.frg";
//    QString vtShaderFile = "G:\\OpenGLWithQML\\OpenGLWithQML\\shaders\\vertex_shader.vtx";
//    myWindow->readShaderFile(vtShaderFile,fgShaderFile);

    qmlRegisterType<MyOpenglWindow>("OpenGLWithQML",1,0,"MyOpenGLWindow");

    QQmlApplicationEngine engine;
//    engine.rootContext()->setContextProperty("myOpenGLWindow",myWindow);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
