import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick 2.5
import QtQuick.Window 2.2

Window {
    visible: true
    width: 640
    height: 480


    Rectangle {
        width: 360;
        height: 300;
        color: "#EEEEEE";

        //delegate下面的所有“可见”元素都会被放到ListView的每个条目中，相当于ListView这个结构体数组的一个
        //成员，默认情况下都会被左靠齐放在条目中，可读性很差，因为都是重叠在一起的。这个时候就需要使用
        //RowLayout将这些元素布局一下
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

                //负责可见元素的布局，让ListView的每个条目都具有可读性，而不是重叠在一起
                //当然了，如果对于每一个可读元素都手动定位分布好，那么也不需要RowLayout参与
                RowLayout {
                    anchors.left: parent.left;
                    anchors.verticalCenter: parent.verticalCenter;
                    spacing: 8;
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
                }
            }
        }

        ListView {
            id: listView;
            anchors.fill: parent;

            delegate: phoneDelegate;

            model: ListModel {
                id: phoneModel;
                ListElement{
                    name: "iPhone 3GS";
                    cost: "1000";
                    manufacturer: "Apple";
                }
                ListElement{
                    name: "iPhone 4";
                    cost: "1800";
                    manufacturer: "Apple";
                }
                ListElement{
                    name: "iPhone 4S";
                    cost: "2300";
                    manufacturer: "Apple";
                }
                ListElement{
                    name: "iPhone 5";
                    cost: "4900";
                    manufacturer: "Apple";
                }
                ListElement{
                    name: "B199";
                    cost: "1590";
                    manufacturer: "HuaWei";
                }
                ListElement{
                    name: "MI 2S";
                    cost: "1999";
                    manufacturer: "XiaoMi";
                }
                ListElement{
                    name: "GALAXY S5";
                    cost: "4699";
                    manufacturer: "Samsung";
                }
            }

            focus: true;

            highlight: Rectangle{
                color: "lightblue";
            }
        }
    }
}

