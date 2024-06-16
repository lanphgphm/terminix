import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.5

// TODO 1.3: detect the border of splitView and confine (clip) the text to wrap inside that border
SplitView{
    id: splitScreen

    property int n_activeScreens: 1
    property int activeScreenIdx: 0
    property ScreenView activeScreen: null
    Component.onCompleted: {
        splitScreen.activeScreen = initialScreen;
    }

    anchors.fill: parent
    orientation: Qt.Horizontal
    clip: true

    SplitView {
        id: initialSplit

        clip: true
        ScreenView {
            id: initialScreen

            SplitView.fillWidth: true
            SplitView.fillHeight: true

            Component.onCompleted: {
                sessionEnded.connect(handleScreenClosure);
            }
        }
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

    function handleScreenClosure() {
        n_activeScreens--;
        if (n_activeScreens === 0) Qt.quit();
    }

    function split(orientation){
        let newScreenComponent = Qt.createComponent("ScreenView.qml");
        let newSplitView = Qt.createQmlObject(`
            import QtQuick.Controls 2.13
            import QtQuick.Layouts 1.5

            SplitView {
                clip: true
                orientation: ${orientation}
            }`,
            splitScreen);

        if (newScreenComponent.status === Component.Ready && newSplitView) {
            let newScreen = newScreenComponent.createObject();
            let oldScreen = splitScreen.takeItem(splitScreen.activeScreenIdx);
            newScreen.sessionEnded.connect(function() { handleScreenClosure(newScreen); });

            newSplitView.addItem(oldScreen);
            newSplitView.addItem(newScreen);

            if(orientation === "Qt.Horizontal"){ // split vertical
                let eachWidth = newSplitView.width / 2;
                oldScreen.preferredWidth = eachWidth;
                newScreen.preferredWidth = eachWidth;
            }
            else { // split horizontal
                let eachHeight = newSplitView.height / 2;
                oldScreen.preferredHeight = eachHeight;
                newScreen.preferredHeight = eachHeight;
            }

            splitScreen.insertItem(splitScreen.activeScreenIdx, newSplitView);
            splitScreen.n_activeScreens++;

            newScreen.forceActiveFocus();
        }
    }

    function splitVertical(){
        split("Qt.Horizontal");
    }

    function splitHorizontal(){
        split("Qt.Vertical");
    }
}
