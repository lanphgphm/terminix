import QtQuick 2.15
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.5

Rectangle {
    id: screenView

    property int defheight: 35
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

            // listView.forceLayout(); 
            // listView.positionViewAtEnd(); // autoscroll down
        }

        function onTerminalSessionEnded() {
            screenView.visible = false;
            sessionEnded();
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
        keyNavigationEnabled: false

        ScrollBar.vertical: ScrollBar {
            id: ybar

            hoverEnabled: true 
            active: hovered || pressed
            orientation: Qt.Vertical
            policy: ScrollBar.AlwaysOn
            stepSize: 0.5

            anchors.top: parent.top 
            anchors.right: parent.right 
            anchors.bottom: parent.bottom 
        }

        Keys.onPressed: (event) => {
            if (event.key === Qt.Key_End) {
                listView.positionViewAtEnd(); 
                event.accepted = true; 
            }
        }

        delegate: Item {
            width: parent? parent.width : 0
            height: model.type === "outputText" ? outputArea.height : defheight

            TextEdit {
                id: outputArea

                readOnly: true
                visible: model.type === "outputText"
                width: screenView.width - 16 // outputArea.contentWidth
                height: outputArea.contentHeight
                padding: 8

                font.family: "monospace"
                font.pointSize: 12
                color: "#d5d5d5"

                focus: true 
                cursorVisible: true
                wrapMode: TextEdit.WordWrap // NoWrap or WordWrap
                textFormat: TextEdit.RichText
                text: model.content
            }

            TextInput {
                id: inputArea

                readOnly: false
                visible: model.type === "inputText"
                width: listView.width - 16
                height: contentHeight > defheight ? contentHeight : defheight
                padding: 8

                font.family: "monospace"
                font.pointSize: 13
                font.bold: true
                color: "#88d22f"

                echoMode: screenView.isEnteringPassword ? TextInput.Password : TextInput.Normal
                text: model.content
                wrapMode: TextInput.Wrap
                cursorVisible: true
                autoScroll: true

                Component.onCompleted: {
                    inputArea.forceActiveFocus(); 
                }

                Keys.onPressed: (event) => { 
                    if (event.modifiers & Qt.ControlModifier) {
                        screenController.handleControlKeyPress(event.key);
                        inputArea.text = "";
                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Up || event.key === Qt.Key_Down) {
                        // forward raw 
                        let rawSequence = event.key === Qt.Key_Up ? "\u001B[A" : "\u001B[B";
                        inputArea.text += rawSequence;
                    }
                    else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        screenController.commandReceivedFromView(text);
                        inputArea.text = "";
                        if (screenView.isEnteringPassword) screenView.isEnteringPassword = false;
                        event.accepted = true;
                    }
                }
            }
        }
    }
}
