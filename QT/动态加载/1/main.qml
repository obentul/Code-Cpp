/* 注意：Loader对应Component，是主窗口的子类，因此对Loader设置z值是有效的，但是对Rectangle设置z无效
*  因为Loader都被覆盖了，Rectangle作为Loader的子元素，理所应当被覆盖
*/

import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Component{              //(!!!)这里定义了，但是是不显示的，如果想显示必须实例化，如果想实例化，必须使用Loader
        id:c1;
        Rectangle{
            id:r1
            height: 100
            width: 100
            color:"red"
            z:10            //这里z不生效，因为Rectangle是Component的子类，是寄生在Component中的，因此会被覆盖

            Button{
                id:b1
                text:"xx"
            }
        }
    }

    Loader{
        id:c1Loader
        visible: true
        z:10                //Loader指向Component，item指向Rectangle，所以应当在Loader中设置z，而不是在item中
        sourceComponent: c1;
        onLoaded: {
            item.color="blue";  //修改后，发现生效
        }
    }

    MainForm {
        visible: true
        anchors.fill: parent
        mouseArea.onClicked: {
            Qt.quit();
        }
    }
}

