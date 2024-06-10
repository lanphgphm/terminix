import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.5

SplitView{
    id: splitScreen

    anchors.fill: parent
    orientation: Qt.Horizontal

    property ScreenView activeScreen: null
    property int activeScreenIdx: 0
    Component.onCompleted: activeScreen = initialScreen

    ScreenView {
        id: initialScreen
        SplitView.fillWidth: true
        SplitView.fillHeight: true
    }

    function setActiveScreen(screen) {
        activeScreen = screen;
        for (let i = 0; i < splitScreen.count; i++){
            if (splitScreen.itemAt(i) === screen){
                activeScreenIdx = i;
                break;
            }
        }
    }

    function splitVertical() {
        let newScreenComponent = Qt.createComponent("ScreenView.qml");
        let newSplitView = Qt.createQmlObject(`
                              import QtQuick.Controls 2.13
                              import QtQuick.Controls 2.13
                              import QtQuick.Layouts 1.5

                              SplitView {
                                  id: newSplit
                                  orientation: Qt.Horizontal
                              }`,

                              splitScreen);
        if (newScreenComponent.status === Component.Ready
            && newSplitView){
            // create newScreen & fetch oldScreen
            let newScreen = newScreenComponent.createObject();
            let oldScreen = splitScreen.takeItem(splitScreen.activeScreenIdx);

            // add activeScreen to newSplitView, then add newScreen to newSplitView
            newSplitView.addItem(oldScreen);
            newSplitView.addItem(newScreen);

            let eachWidth = newSplitView.width / 2;
            oldScreen.width = eachWidth;
            newScreen.width = eachWidth;

            // replace activeScreen with new SplitView
            splitScreen.insertItem(splitScreen.activeScreenIdx, newSplitView);

            // update activeScreen and focus on newScreen
            setActiveScreen(newScreen);
            newScreen.forceActiveFocus();
        }
    }

    function splitHorizontal() {
        let newScreenComponent = Qt.createComponent("ScreenView.qml");
        let newSplitView = Qt.createQmlObject(`
                              import QtQuick.Controls 2.13
                              import QtQuick.Controls 2.13
                              import QtQuick.Layouts 1.5

                              SplitView {
                                  id: newSplit
                                  orientation: Qt.Vertical
                              }`,

                              splitScreen);
        if (newScreenComponent.status === Component.Ready
            && newSplitView){
            // create newScreen & fetch oldScreen
            let newScreen = newScreenComponent.createObject();
            let oldScreen = splitScreen.takeItem(splitScreen.activeScreenIdx);

            // add activeScreen to newSplitView, then add newScreen to newSplitView
            newSplitView.addItem(oldScreen);
            newSplitView.addItem(newScreen);

            let eachHeight = newSplitView.height / 2;
            oldScreen.height = eachHeight;
            newScreen.height = eachHeight;


            // replace activeScreen with new SplitView
            splitScreen.insertItem(splitScreen.activeScreenIdx, newSplitView);

            // update activeScreen and focus on newScreen
            setActiveScreen(newScreen);
            newScreen.forceActiveFocus();
        }
    }

}
