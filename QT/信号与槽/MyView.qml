import QtQuick 2.0
import QtQuick.Controls 1.2

Rectangle {
    id:r1
    signal hhhh();



    Button{
        id:b1
        anchors.bottom:parent.bottom
    }

    Component.onCompleted: {
        r1.hhhh.connect(ffff);
    }

    function ffff(){
        console.log("ffff");
    }


}
