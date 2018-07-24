import QtQuick 2.5
import QtQuick.Window 2.2
import Qt.labs.controls 1.0
import QtQuick.Controls 1.4

Window {
    id:w1
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    property var i:3
    property var j:2
    property var ok_x:200
    property var ok_y:200

    property var no_x:400
    property var no_y:200
    property var no_100:100




    Label{
        id:t1

        anchors.horizontalCenter:parent.horizontalCenter

        y: 73

        font.pixelSize: 30
        color:"red"

        text:"xxx，你是S B么"
    }

    Button{
        id:b_no
        x:no_x
        y:no_y
        text:"不是"

        MouseArea{
            anchors.rightMargin: 0
            anchors.bottomMargin: 0
            anchors.leftMargin: 0
            anchors.topMargin: 0
            anchors.fill:parent
            enabled:trye
            hoverEnabled: true
            onHoveredChanged: {

                if(i!=0){
                    (i===1)? b_no.y=no_y : b_no.y=no_100;
                    i--;
                }
                else if(i==0)
                {
                    b_no.x=ok_x;
                    b_no.y=ok_y;
                    b_ok.x=no_x;
                    b_ok.y=no_y;
                }
                else{
                    t1.text="确定不是？？？？？"
                }
            }
            onPressed: {
                t1.text="确定不是？？？？？"
                t1.font.pixelSize++;
                t1.color=Qt.rgba(Math.random(), Math.random(),Math.random());
            }
        }
    }

    Button{
        id:b_ok
        x:ok_x
        y:ok_y
        text:"是"

        onClicked: {
            if(j!=1)
            {
                t1.text="good,再点一次"
                j--;
            }
            if(j==1)
            {
                w1.close();
            }
        }
    }

    Component.onCompleted: {
        //w1.showFullScreen();
    }

}
