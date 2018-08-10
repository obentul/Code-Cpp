//MouseArea的嵌套用法：MouseArea仅作用于自己的parent，如果propagateComposedEvents被设置为true，那么会把信号再向上一层传递，如果上一层的MouseArea也设置了，那么
//继续向上一层传递


import QtQuick 2.5
import QtQuick.Window 2.2

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

        Rectangle {
            color: "red"
            width: 200; height: 200

            MouseArea {
                anchors.fill: parent
                onClicked: console.log("clicked red")
            }

            Rectangle {
                color: "yellow"
                width: 100; height: 100

                MouseArea {
                    anchors.fill: parent
                    propagateComposedEvents: true
                    onClicked: {
                        console.log("clicked yellow")
                        mouse.accepted = false
                    }
                }

                Rectangle {
                    color: "blue"
                    width: 50; height: 50

                    MouseArea {
                        anchors.fill: parent
                        propagateComposedEvents: true
                        onClicked: {
                            console.log("clicked blue")
                            mouse.accepted = false
                        }
                    }
                }
            }
        }
}

