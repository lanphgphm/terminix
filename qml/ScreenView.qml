import QtQuick 2.15
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.5
import com.terminix 1.0 // needed to import ScreenController as QML

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
            let newContent = listModel.get(0).content + result;
            console.log(newContent); // DEBUG
            listModel.setProperty(0, "content", newContent);

            if (!listView.userScrolled)
                listView.positionViewAtEnd(); // autoscroll down
        }
    }

    ListView {
        id: listView
        model: listModel

        anchors.fill: parent
        width: parent ? parent.width : 0
        height: parent.height

        property bool userScrolled: false
        onContentYChanged: {
            if (Math.abs(contentHeight - height - contentY) < 2) {
                userScrolled = false; // User is at the bottom, allow autoscroll
            }
        }

        ScrollBar.vertical: ScrollBar {
            onActiveChanged: {
                if (active) {
                    listView.userScrolled = true;
                }
            }
        }

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
                textFormat: TextEdit.RichText
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
                    inputArea.text = "";
                }
            }
        }
    }
}

