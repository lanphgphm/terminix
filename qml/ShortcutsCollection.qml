import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.5

Item {
    id: shortcutsCollection

    property var appId
    property var rootScreenId

    Shortcut {
        sequence: "Ctrl+M"
        onActivated: {
            if (appId.visibility !== Window.Windowed){
                appId.visibility = Window.Windowed;
            }
            else {
                appId.visibility = Window.Maximized;
            }
        }
    }
}
