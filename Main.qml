import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.5
import "./qml"

ApplicationWindow {
    id: app

    width: 800
    height: 600
    title: "Terminix"
    visible: true
    visibility: Window.Windowed

    // menuBar: TopMenuBar {}

    SplitScreenView {
        id: rootSplitScreen

        anchors.fill: parent
    }

    ShortcutsCollection {
        appId: app
        rootScreenId: rootSplitScreen

        // contains Ctrl+Shift+E for vertical splitting
        // contains Ctrl+Shift+O for horizontal splitting
    }
}
