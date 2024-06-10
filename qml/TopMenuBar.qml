import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: root
    height: 30
    width: 600
    visible: true

    MenuBar{
        Menu {
            title: "File"

            MenuItem {
                text: "New"
                onTriggered: console.log("New file")
            }
            MenuItem {
                text: "Open"
                onTriggered: console.log("Open file")
            }
            MenuItem {
                text: "Save"
                onTriggered: console.log("Save file")
            }
            MenuSeparator {}
            MenuItem {
                text: "Exit"
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: "Window"

            MenuItem {
                text: "Full screen"
                onTriggered: console.log("Going full screen")
            }
            MenuItem {
                text: "Split vertical"
                onTriggered: console.log("Split vertical to the right")
            }
            MenuItem {
                text: "Split horizontal"
                onTriggered: console.log("Split horizontal to bottom")
            }
            MenuSeparator {}
            MenuItem {
                text: "Preferences"
                onTriggered: console.log("Opening preferences wizard")
            }
        }

        Menu {
            title: "About"

            MenuItem {
                text: "Synopsis"
                onTriggered: console.log("Returning Terminix synopsis")
            }
            MenuItem {
                text: "Github"
                onTriggered: console.log("Redirecting to Github")
            }

        }
    }

    // MouseArea {
    //     id: mouseArea
    //     hoverEnabled: true
    //     anchors.fill: parent
    //     onEntered: root.visible = true
    //     onExited: root.visible = false
    // }
}

