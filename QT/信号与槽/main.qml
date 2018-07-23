import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.2

Window {
    visible: true
    width: 640
    height: 480

    title: qsTr("Hello World")

    Text{
        id:text1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top:parent.top
        text:"one"
        color:"blue"
    }

    Text{
        id:text2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top:parent.top
        text:"one"
        color:"blue"
    }


    MyView{
        id:v1
        z:10
        Button{
            id:b1
            text:"xxx"

            //也可以在这里定义clicked信号的处理函数(匿名)，
            //onClicked:{
            //    text1.color = Qt.rgba(Math.random(),
            //                           Math.random(),Math.random());
            //    text2.color = Qt.rgba(Math.random(),
            //                           Math.random(),Math.random());
            //}
        }

        //(!!!)可以在任何地方，给信号添加新的处理函数
        //(!!!)在QT中，不能再代码块外部做动作，所以这里就把处理函数注册放在Component.onCompleted中
        Component.onCompleted: {
            v1.hhhh.connect(gggg);
        }
    }

    //
    Connections{
        target:b1
        onClicked:{
           text1.color = Qt.rgba(Math.random(),
                                  Math.random(),Math.random());
           text2.color = Qt.rgba(Math.random(),
                                  Math.random(),Math.random());
           v1.hhhh();                                               //触发v1的信号，信号处理函数注册在MyView中
        }
    }

    //定义函数gggg，用来挂到MyView的信号hhhh上，在MyView的实现中，已经挂了一个ffff在hhhh上了
    function gggg(){
        console.log("gggg");
    }
}
