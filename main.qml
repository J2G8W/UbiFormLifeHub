import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Text {
        id: text1
        x: 169
        y: 106
        width: 164
        height: 69
        text: qsTr("Hello its me I've been wondering")
        font.pixelSize: 12
    }
}
