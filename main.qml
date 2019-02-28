import QtQuick 2.11
import QtQuick.Window 2.11

import OpenGLWithQML 1.0

Window {
    visible: true
    width: 600
    height: 600
    title: qsTr("Hello World")

    MyOpenGLWindow{
        anchors.fill: parent
        focus: true
        Keys.onPressed: {
            if (event.key === Qt.Key_Up){
                console.log("Qt.Key_Up")
                mixValue+=0.1
            }
            if (event.key === Qt.Key_Down){
                console.log("Qt.Key_Down")
                mixValue-=0.1
            }
        }
//        Rectangle{
//            anchors.centerIn: parent
//            width: 10
//            height: 10
//            color:"red"
//            MouseArea{
//                anchors.fill: parent
//                onClicked: {
//                    console.log("????????")
//                }
//            }
//        }
    }

}
