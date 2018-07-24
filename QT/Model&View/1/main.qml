/*在MyView.qml中定义delegate，在main.qml中定义ListView，并引用MyView.qml中的delegate*/

import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.2

Window {
    visible: true
    width: 640
    height: 480

    //MyView组件定义在MyView.qml中，因为MyView.qml只相当于定义了组件格式，没有实例化(参见 动态加载/2)，所以这里只要实例化一下
    //获得一个可用的实例，然后给ListView使用就好了
    MyView{
        id:phoneDelegate;
    }

    ListView {
        id: listView;
        anchors.fill: parent;

        //ListView的delegate！！！！只！！！！接收Component类型，所以，如果组件外层包裹了Rectangle，要么把外层剥掉，要么直接
        //使用内存Component的id
        delegate: phoneDelegate;

        //这里直接就是使用ListModel
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
