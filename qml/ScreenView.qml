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
        ListElement { type: "outputText"; content: "" }
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

        function onTerminalSessionEnded() {
            Qt.quit();
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
                // textFormat: TextEdit.PlainText
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

                Keys.onPressed: (event) => { //   --------------bitmask----------------
                    if (event.key === Qt.Key_C && (event.modifiers & Qt.ControlModifier)) {
                        screenController.handleControlKeyPress(Qt.Key_C);
                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Z && (event.modifiers & Qt.ControlModifier)) {
                        screenController.handleControlKeyPress(Qt.Key_Z);
                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Backslash && (event.modifiers & Qt.ControlModifier)) {
                        screenController.handleControlKeyPress(Qt.Key_Backslash);
                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_S && (event.modifiers & Qt.ControlModifier)) {
                        screenController.handleControlKeyPress(Qt.Key_S);
                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Q && (event.modifiers & Qt.ControlModifier)) {
                        screenController.handleControlKeyPress(Qt.Key_Q);
                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        screenController.commandReceivedFromView(text);
                        text = "";
                        event.accepted = true;
                    }
                }
            }
        }
    }
}

