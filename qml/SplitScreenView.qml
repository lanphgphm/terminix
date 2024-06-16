import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.5

// TODO 1.3: detect the border of splitView and confine the text to wrap inside that border
SplitView{
    id: splitScreen

    anchors.fill: parent
    orientation: Qt.Horizontal

    // -------TODO 1.4: keep track of all opened screens----------
    // && conditional Qt.quit() if only 1 screen left visible
    property ScreenView activeScreen: null
    property int activeScreenIdx: 0
    Component.onCompleted: activeScreen = initialScreen
    // -------------------------------------------------------

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

        // let newScreenComponent = Qt.createComponent("ScreenView.qml");
        // if (newScreenComponent.status === Component.Ready){
        //     let newScreen = newScreenComponent.createObject();
        //     splitScreen.addItem(newScreen);
        //     setActiveScreen(newScreen);
        //     newScreen.forceActiveFocus();
        // }
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

        // let newScreenComponent = Qt.createComponent("ScreenView.qml");
        // if (newScreenComponent.status === Component.Ready){
        //     let newScreen = newScreenComponent.createObject();
        //     splitScreen.addItem(newScreen);
        //     setActiveScreen(newScreen);
        //     newScreen.forceActiveFocus();
        // }
    }
}

//------------------------------------------------
// RowLayout {
//     anchors.fill: parent
//     spacing: 0
//     Rectangle {
//         Layout.preferredWidth: parent.width / 2
//         Layout.fillHeight: true
//         color: "green"
//     }
//     Rectangle {
//         Layout.fillWidth: true
//         Layout.fillHeight: true
//         color: "yellow"
//     }
// }
//-------------------------------------------------

// Rectangle {
//     height: 20
//     width: 300
//     color: "green"
//     GridLayout {
//         anchors.fill: parent
//         columns: 2
//         Rectangle {
//             Layout.fillWidth: true
//             Layout.fillHeight: true
//             color: "red"
//         }
//         Rectangle {
//             Layout.fillWidth: true
//             Layout.fillHeight: true
//             color: "blue"
//         }
//     } //GridLayout
// }
