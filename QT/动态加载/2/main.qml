import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    color:"transparent"

    MyItem{             //MyItem已经是一个类了，可以直接使用，用起来和Button一样
        width: 300       //可以重新复制
        z:10            //放到顶层
        color: "red"    //原本没赋值的属性，这里可以初始化

        Button{         //在MyItem.qml中定义了的
            text:"yyy"  //覆盖旧值
        }

        Button{         //(!!!)即使原本模板中只有一个Button，在实例化的时候依旧可以自己加一个进去
            text:"aaa"
            anchors.bottom: parent.bottom
        }

        Label{
            text:"i'm a label"       //(!!!)即使原本模板中压根没有Label元素，在实例化的时候依旧可以自己添加
            anchors.centerIn: parent
        }

    }


    MainForm {
        anchors.fill: parent
        mouseArea.onClicked: {
            Qt.quit();
        }
    }
}

