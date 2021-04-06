import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13
import QtQml 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Slider {
        id: slider
        x: 28
        y: 12
        stepSize: 1
        to: 255
        value: 20
        onValueChanged: {
            dtuSimulator.handleSpeedChanged(value)
            text1.text = "Speed: " + value
        }
    }

    Text {
        id: text1
        x: 278
        y: 14
        width: 178
        height: 36
        text: qsTr("Speed:" + slider.value)
        font.pixelSize: 24
    }

    Connections {
        target: socketManager
        onTextChanged: {
            textArea.text = textArea.text + text
        }


    }

    Button {
        id: button
        x: 46
        y: 440
        width: 93
        height: 24
        text: qsTr("Clear")
        onClicked: {
            textArea.text = ""
        }
    }

    Rectangle {
        id: rectangle
        x: 28
        y: 51
        width: 585
        height: 377
        color: "#ffffff"
        border.width: 2

        TextArea {
            id: textArea
            text: qsTr("Text Area")
            anchors.fill: parent
        }
    }
}
