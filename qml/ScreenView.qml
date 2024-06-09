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
        id: viewListModel
        ListElement { type: "inputText"; content: "" }
    }

    ListView {
        id: viewListView
        model: viewListModel

        anchors.fill: parent
        width: parent ? parent.width : 0
        height: parent.height

        delegate: Item {
            width: parent? parent.width : 0
            height: model.type === "inputText" ? 30 : outputArea.height

            TextEdit {
                id: outputArea

                readOnly: true
                visible: model.type === "outputText"
                width: parent ? parent.width : 0
                height: outputArea.contentHeight

                font.family: "monospace"
                font.pointSize: 12
                color: "white"

                wrapMode: TextEdit.Wrap
                text: model.content

                Connections {
                    target: screenController
                    function onResultReadySendToView(result) {
                        // displayResult(QString result) -- syntactically here
                        viewListModel.remove(index);
                        viewListModel.append({"type": "outputText", "content": result});
                        viewListModel.append({"type": "inputText", "content": ""});

                        viewListView.positionViewAtEnd();
                    }
                }

                Component.onCompleted: {
                    if (visible) forceActiveFocus();
                }
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
                    screenController.commandReceivedFromView(text);
                }
            }
        }
    }
}



























