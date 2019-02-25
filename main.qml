import QtQuick 2.11
import QtQuick.Window 2.11

import OpenGLWithQML 1.0

Window {
    visible: true
    width: 300
    height: 300
    title: qsTr("Hello World")

    MyOpenGLWindow{
        anchors.fill: parent
    }

}
