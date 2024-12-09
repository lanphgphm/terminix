import QtQuick 2.15
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.5

Rectangle {
    id: screenView

    property int defheight: 25
    property bool isEnteringPassword
    signal sessionEnded()

    color: "#282a2e"
    visible: true
    Layout.fillWidth: true
    Layout.fillHeight: true

    function clearScreen() {
        // -----TODO 1.1: implement clear screen--------
        console.log("Implement later :D");
        // -----------------------------------------
    }

    ListModel {
        id: listModel
        ListElement { type: "outputText"; content: "" }
        ListElement { type: "inputText"; content: "" }
    }

    Connections {
        target: screenController
        function onResultReadySendToView(result) {
            // --------IMPROVE 1.0: need a better way to detect password :)----------------------
            const passwordSynonyms = ['password', 'passwd', 'pass', 'authen',
                                      'passcode', 'secret', 'passphrase',
                                      'key', 'pin', 'authentication', 'token',
                                      'credential', 'access code', 'security code'];
            const regex = new RegExp(passwordSynonyms.join('|'), 'i'); // i = ignore casing
            if (regex.test(result)) {
                screenView.isEnteringPassword = true;
            }
            else screenView.isEnteringPassword = false;
            // ---------------------------------------------------------------------------

            let newContent = listModel.get(0).content + result;
            listModel.setProperty(0, "content", newContent);

            listView.forceLayout(); 
            listView.positionViewAtEnd(); // autoscroll down
        }

        function onTerminalSessionEnded() {
            screenView.visible = false;
            sessionEnded();
        }

        function onShowCommand(command) {
            listView.currentItem.setCommand(command);
        }
    }

    Connections {
        target: screenView
        function onSessionEnded(){
            Qt.quit()
        } 
    }

    ListView {
        id: listView
        model: listModel

        anchors.fill: parent
        width: parent ? parent.width : 0
        height: parent.height
        currentIndex: 1

        delegate: Item {
            width: parent? parent.width : 0
            height: model.type === "outputText" ? outputArea.height : defheight

            TextEdit {
                id: outputArea

                readOnly: true
                visible: model.type === "outputText"
                width: parent ? parent.width : 0
                height: outputArea.contentHeight

                font.family: "monospace"
                font.pointSize: 12
                color: "#d5d5d5"

                wrapMode: TextEdit.Wrap
                textFormat: TextEdit.RichText
                text: model.content
            }

            TextInput {
                id: inputArea

                readOnly: false
                focus: true
                visible: model.type === "inputText"
                width: parent ? parent.width : 0
                height: contentHeight > defheight ? contentHeight : defheight

                font.family: "monospace"
                font.pointSize: 13
                font.bold: true
                color: "#88d22f"

                echoMode: screenView.isEnteringPassword ? TextInput.Password : TextInput.Normal
                text: model.content
                wrapMode: TextInput.Wrap

                Component.onCompleted: {
                    inputArea.forceActiveFocus(); 
                }

                Keys.onPressed: (event) => { //   --------------bitmask----------------
                    if (event.key === Qt.Key_C && (event.modifiers & Qt.ControlModifier)) {
                        screenController.handleControlKeyPress(Qt.Key_C);
                        inputArea.text = "";
                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_L && (event.modifiers & Qt.ControlModifier)) {
                        clearScreen();
                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Z && (event.modifiers & Qt.ControlModifier)) {
                        screenController.handleControlKeyPress(Qt.Key_Z);
                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        screenController.commandReceivedFromView(text);
                        text = "";
                        if (screenView.isEnteringPassword) screenView.isEnteringPassword = false;
                        event.accepted = true;
                    }
                }
            }

            function setCommand(command) {
                inputArea.text = command;
            }
        }
    }
}

