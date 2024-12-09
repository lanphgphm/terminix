#include "screencontroller.h"
#include <QtGlobal> // qDebug
#include <QtDebug> // qDebug overload

ScreenController::ScreenController(QObject* parent)
    : QObject(parent)
    , m_ptty(new Ptty(this))
{
    // constructor
    qDebug() << "Constructing ScreenController...";
    m_ptty->start();

    QObject::connect(m_ptty,
                     &Ptty::resultReceivedFromShell,
                     this,
                     &ScreenController::resultReceivedFromPty);
    QObject::connect(this,
                     &ScreenController::commandReadySendToPty,
                     m_ptty,
                     &Ptty::executeCommand);

}

ScreenController::~ScreenController() {
    // destructor
}

void ScreenController::commandReceivedFromView(QString command){
    emit commandReadySendToPty(command);
}

void ScreenController::resultReceivedFromPty(QString result){
    if (result.contains("Process has exited")){
        // handle terminal closure by informing QML
        qDebug("Terminal session has ended\n");
        emit terminalSessionEnded();
    }
    else{
        QString htmlResult = ansiToHtml(result);
        emit resultReadySendToView(htmlResult);
    }
}

void ScreenController::handleControlKeyPress(Qt::Key key){
    if (keySignalMap.contains(key)) {
        int signal = keySignalMap[key];
        m_ptty->sendSignal(signal);
    }
}

QString ScreenController::processPrompt(const QString& ansiText){
    // Processing prompt
    QString filteredAnsiText = ansiText;

    static QRegularExpression promptPattern(R"(nutshell>)");
    QRegularExpressionMatch promptMatch = promptPattern.match(filteredAnsiText);
    if (promptMatch.hasMatch()) {
        QString promptSymbol = promptMatch.captured(2); 

        filteredAnsiText.replace(promptMatch.captured(0),
                                     "<span style=\"color:#cd7db7;\"><b>"
                                         + promptMatch.captured(0).toHtmlEscaped()
                                         + "</b></span>");
    }

    return filteredAnsiText;
}

QString ScreenController::ansiToHtml(const QString& ansiText) {
    QString htmlText = "<pre>"; // Preserve whitespace
    QString filteredAnsiText = processPrompt(ansiText);

    // Match all ANSI escape sequences
    static QRegularExpression ansiPattern("\\x1b\\[([0-9;?]*)([a-zA-Z])");
    QRegularExpressionMatchIterator i = ansiPattern.globalMatch(filteredAnsiText);
    int lastPosition = 0;
    QStringView view(filteredAnsiText);

    QStringList openTags;
    QString currentStyle;

    while (i.hasNext()) {
        auto match = i.next();
        QString parameters = match.captured(1);
        QString command = match.captured(2);

        QString textSegment = view.mid(lastPosition, match.capturedStart() - lastPosition).toString();
        htmlText += textSegment;

        lastPosition = match.capturedEnd();
    }

    htmlText += view.mid(lastPosition).toString();
    while (!openTags.isEmpty()) {
        htmlText += openTags.takeLast();
    }
    htmlText += "</pre>";

    return htmlText;
}
