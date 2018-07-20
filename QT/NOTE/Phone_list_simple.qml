import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Window {
    visible: true
    width: 800
    height: 600
    title: qsTr("Hello World")

    Rectangle {
        width: 800;
        height: 600;
        color: "#EEEEEE";

        Component {
            id: phoneDelegate;
            Item {
                id: wrapper;
                width: parent.width;
                height: 30;

                MouseArea {
                    anchors.fill: parent;
                    onClicked: wrapper.ListView.view.currentIndex = index;
                }

                /*定义一个行样本，行中的元素(此例中的3个text)按照对其格式在行内被排列对其，spacing属性表明了元素之间的距离，比如：
                *   |---------------------------------------|
                *   |text1   text2   text3                  |
                *   |---------------------------------------|
                *   1和2之间是spacing，2和3之间是spacing，anchors.left: parent.left表明起始位置是和parent对其的
                */
                RowLayout {
                    anchors.left: parent.left;
                    anchors.verticalCenter: parent.verticalCenter;
                    spacing: 5;
                    Text {
                        id: col1;
                        text: name;
                        color: wrapper.ListView.isCurrentItem ? "red" : "black";
                        font.pixelSize: wrapper.ListView.isCurrentItem ? 22 : 18;
                        Layout.preferredWidth: 120;
                    }

                    Text {
                        text: cost;
                        color: wrapper.ListView.isCurrentItem ? "red" : "black";
                        font.pixelSize: wrapper.ListView.isCurrentItem ? 22 : 18;
                        Layout.preferredWidth: 80;
                    }

                    Text {
                        text: manufacturer;
                        color: wrapper.ListView.isCurrentItem ? "red" : "black";
                        font.pixelSize: wrapper.ListView.isCurrentItem ? 22 : 18;
                        Layout.fillWidth: true;
                    }

                    Button{
                        text:x
                        width: 5
                        height: 5
                        onClicked: {
                            console.log(index);
                        }
                    }
                }
            }
        }

        ListView {
            id: listView;
            anchors.leftMargin: 0
            anchors.fill: parent;

            delegate: phoneDelegate;

            model: ListModel {
                id: phoneModel;
                ListElement{
                    name: "iPhone 3GS";
                    cost: "1000";
                    manufacturer: "Apple";
                    x:"1"
                }
                ListElement{
                    name: "iPhone 4";
                    cost: "1800";
                    manufacturer: "Apple";
                    x:"2"
                }
                ListElement{
                    name: "iPhone 4S";
                    cost: "2300";
                    manufacturer: "Apple";
                    x:"3"
                }
                ListElement{
                    name: "iPhone 5";
                    cost: "4900";
                    manufacturer: "Apple";
                    x:"4"
                }
                ListElement{
                    name: "B199";
                    cost: "1590";
                    manufacturer: "HuaWei";
                    x:"5"
                }
                ListElement{
                    name: "MI 2S";
                    cost: "1999";
                    manufacturer: "XiaoMi";
                    x:"6"
                }
                ListElement{
                    name: "GALAXY S5";
                    cost: "4699";
                    manufacturer: "Samsung";
                    x:"7"
                }
            }

            focus: true;

            highlight: Rectangle{
                color: "lightblue";
            }
        }
    }
}

