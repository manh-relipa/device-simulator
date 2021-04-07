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
        value: 0
        onValueChanged: {
            dtuSimulator.handleSpeedChanged(value)
            text1.text = "Speed: " + value
        }

        Component.onCompleted: {
            dtuSimulator.handleSpeedChanged(value)
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

    Connections {
        target: dtuSimulator
        onTextChanged: {
            textArea.text = textArea.text + text
        }


    }

    Button {
        id: button
        x: 46
        anchors.top: rectangle.bottom
        anchors.topMargin: 20
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
        width: parent.width - 60
        height: parent.height - 120
        color: "#ffffff"
        border.width: 2

        Flickable {
            id: flickable
            anchors.fill: parent

            TextArea.flickable: TextArea {
                id: textArea
                wrapMode: TextArea.Wrap
            }

            ScrollBar.vertical: ScrollBar { }
        }
    }
}
