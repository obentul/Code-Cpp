import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")


    Item {
       width: 600
       height: 600

       //Model
       ListModel {
          id: objModel
          Component.onCompleted: {
              objModel.append({"name":"Zero","level":0,"subNode":[]})
              objModel.append({"name":"One","level":0,"subNode":[]})
              objModel.append({"name":"Two","level":0,"subNode":[]})
              objModel.get(1).subNode.append({"name":"Three","level":1,"subNode":[]})
              objModel.get(1).subNode.append({"name":"Four","level":1,"subNode":[]})
              objModel.get(1).subNode.get(0).subNode.append({"name":"Five","level":2,"subNode":[]})
          }
       }

       //Delegate
       Component {
             id: objRecursiveDelegate
             Column {
                id: objRecursiveColumn
                MouseArea {
                   width: objRow.implicitWidth
                   height: objRow.implicitHeight
                   onDoubleClicked: {
                      for(var i = 1; i < parent.children.length - 1; ++i) {
                         parent.children[i].visible = !parent.children[i].visible
                      }
                   }
                   Row {
                      id: objRow
                      Item {
                         height: 1
                         width: model.level * 20
                      }
                      Text {
                         text: (objRecursiveColumn.children.length > 2 ?
                                  objRecursiveColumn.children[1].visible ?
                                  qsTr("-  ") : qsTr("+ ") : qsTr("   ")) + model.name
                         color: objRecursiveColumn.children.length > 2 ? "blue" : "green"
                      }
                   }
                }
                Repeater {
                   model: subNode
                   delegate: objRecursiveDelegate
                }
             }
          }

       //View
          ListView {
             anchors.fill: parent
             model: objModel
             delegate: objRecursiveDelegate
          }
    }
}

