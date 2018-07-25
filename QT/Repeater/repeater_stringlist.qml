import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick 2.5
import QtQuick.Window 2.2

Window {
    visible: true
    width: 640
    height: 480


    Rectangle {
        width: 300;
        height: 200;
        color: "#EEEEEE";

        Row {
            anchors.centerIn: parent;
            spacing: 8;
            Repeater {
                model: ["Hello", "Qt", "Quick"];    //使用列表中的值来实例化紧接着的Text
                Text {
                    color: "blue";
                    font.pointSize: 18;
                    font.bold: true;
                    verticalAlignment: Text.AlignVCenter;
                    horizontalAlignment: Text.AlignHCenter;
                    text: modelData;
                }
            }
        }
    }
 }
