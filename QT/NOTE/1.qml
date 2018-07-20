import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Window{
    id: window1
    visible: true
    width: 400
    height: 200
    color:"transparent"
    opacity:1
    title: "xxxxxxxxxxxxxxxx"
    property bool isPsdVis: true



    Rectangle{
        width: 400
        height: 200

        Image {
                id: img0
                width: 24
                height:24
                anchors.top: parent.top
                anchors.topMargin: 7
                anchors.left: parent.left
                anchors.leftMargin: 6
                visible: false
                source: "./tree_icon_favorite.png"
             }

        Button{
            id:favorite
            width:24
            height: 24

            anchors.verticalCenter: img0.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin:1
            style: ButtonStyle{
                background: Item{
                    anchors.fill: parent
                    Image{
                        source:isPsdVis? "./tree_icon_favorite.png":"./tree_icon_favorite_h.png"
                    }
                }
            }
                onClicked:{
                    isPsdVis=!isPsdVis
                }
            }
        }
}

