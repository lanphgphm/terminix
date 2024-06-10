import QtQuick 2.15
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.5

Rectangle {
    id: screenView

    color: "black"
    visible: true

    Layout.fillWidth: true
    Layout.fillHeight: true

    ListModel {
        id: listModel
        ListElement { type: "outputText"; content: "Terminal initialized..." }
        ListElement { type: "inputText"; content: "" }
    }

    Connections {
        target: screenController
        function onResultReadySendToView(result) {
            // displayResult(QString result) -- syntactically here
            let newContent = listModel.get(0).content + "\n" + result; // \n will disrupt data longer than BUFFERSIZE in pty
            console.log(newContent);
            listModel.setProperty(0, "content", newContent);

            listView.positionViewAtEnd(); // not scrolling down
            listView.forceLayout(); // also not scrolling down
        }
    }

    ListView {
        id: listView
        model: listModel

        anchors.fill: parent
        width: parent ? parent.width : 0
        height: parent.height


        delegate: Item {
            width: parent? parent.width : 0
            height: model.type === "inputText" ? 30 : outputArea.height

            TextEdit {
                id: outputArea

                property bool isConnected: false

                readOnly: true
                visible: model.type === "outputText"
                width: parent ? parent.width : 0
                height: outputArea.contentHeight

                font.family: "monospace"
                font.pointSize: 12
                color: "white"

                wrapMode: TextEdit.Wrap
                // textFormat: TextEdit.RichText
                text: model.content
            }

            TextInput {
                id: inputArea

                readOnly: false
                visible: model.type === "inputText"
                width: parent ? parent.width : 0
                height: contentHeight > 30 ? contentHeight : 30

                font.family: "monospace"
                font.pointSize: 13
                font.bold: true
                color: "green"

                wrapMode: TextInput.Wrap
                text: model.content

                onAccepted: {
                    // commandEntered(QString command) -- syntactically here
                    screenController.commandReceivedFromView(inputArea.text);
                    inputArea.forceActiveFocus();
                }

                Component.onCompleted: {
                    inputArea.forceActiveFocus();
                }
            }
        }
    }
}

