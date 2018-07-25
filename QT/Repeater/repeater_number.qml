import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick 2.5
import QtQuick.Window 2.2

Window {
    visible: true
    width: 640
    height: 480


    Rectangle {
        width: 400;
        height: 200;
        color: "#EEEEEE";

        RowLayout {
            anchors.fill: parent;
            spacing: 4;
            Repeater {
                model: 8;   //实例化8个model，每个的格式都是下面这个Rectangle
                Rectangle {
                    width: 46;
                    height: 30;
                    color: "steelblue";
                    Text {
                        anchors.fill: parent;
                        color: "black";
                        font.pointSize: 14;
                        verticalAlignment: Text.AlignVCenter;
                        horizontalAlignment: Text.AlignHCenter;
                        text: index;
                    }
                }
            }
        }
    }

 }

