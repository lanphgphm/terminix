import QtQuick 2.15
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.5
import com.terminix 1.0

Rectangle {
    id: screenView

    // 1 controller instance per 1 view instance
    property ScreenController screenController: ScreenController{}

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
            // virtual function: displayResult(QString result)
            let newContent = listModel.get(0).content + "\n" + result; // \n will disrupt data longer than BUFFERSIZE in pty
            console.log(newContent); // DEBUG
            listModel.setProperty(0, "content", newContent);

            listView.positionViewAtEnd(); // autoscroll down
            // listView.forceLayout();
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
                // only use this when ScreenController::ansiToHtml() works :)
                // textFormat: TextEdit.RichText
                // using PlainText by default
                textFormat: TextEdit.PlainText
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
                    // virtual function: commandEntered(QString command)
                    screenController.commandReceivedFromView(inputArea.text);
                }

                Component.onCompleted: {
                    inputArea.forceActiveFocus();
                }
            }
        }
    }
}

