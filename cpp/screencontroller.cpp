#include "screencontroller.h"
#include <QtGlobal> // qDebug
#include <QtDebug> // qDebug overload

ScreenController::ScreenController(QObject* parent)
    : QObject(parent), m_ptty(new Ptty(this))
{
    // constructor
    m_ptty->start();

    QObject::connect(m_ptty,
                     &Ptty::resultReceivedFromBash,
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
        emit resultReadySendToView("Terminal session has ended\n"); // saying goodbye to user
        emit terminalSessionEnded();
    }
    else{
        QString htmlResult = ansiToHtml(result);
        emit resultReadySendToView(htmlResult);
    }
}

QString ScreenController::ansiToHtml(const QString& ansiText){
    // Devil's formatting language: https://en.wikipedia.org/wiki/ANSI_escape_code
    QString htmlText = "<pre>"; // preserve whitespace
    static QRegularExpression pattern("\\x1b\\[([0-9;]+)m");
    QRegularExpressionMatchIterator i = pattern.globalMatch(ansiText);
    int lastPosition = 0;
    QStringView view(ansiText);

    bool spanTag = false;
    bool boldTag = false;

    while(i.hasNext()){
        auto match = i.next();
        QString code = match.captured(1);
        QStringList codeList = code.split(';');

        QString textSegment = view.mid(lastPosition, match.capturedStart() - lastPosition).toString();
        htmlText += textSegment;

        bool reset = false;
        QString htmlStartTag, htmlEndTag;

        for (const QString& code: codeList){
            int num = code.toInt();
            if (num == 0) {
                reset = true;
            }
            if (num == 1) {
                htmlStartTag.append("<b>");
                htmlEndTag.prepend("</b>");
                boldTag = true;
            }
            if (colorMap.contains(num)) {
                htmlStartTag.append("<span style=\"color:" + colorMap[num] + "\">");
                htmlEndTag.prepend("</span>");
                spanTag = true;
            }
        }

        if (reset) {
            if (spanTag) htmlText.append("</span>");
            if (boldTag) htmlText.append("</b>");
            htmlStartTag.clear();
        }
        htmlText += htmlStartTag;
        lastPosition = match.capturedEnd();
    }
    htmlText += view.mid(lastPosition).toString();
    htmlText += "</pre>";

    htmlText.replace("</pre><pre>", "");

    return htmlText;
}

void ScreenController::handleControlKeyPress(Qt::Key key){
    qDebug("You just pressed Ctrl key\n");
    if (keySignalMap.contains(key)) {
        int signal = keySignalMap[key];
        m_ptty->sendSignal(signal);
    }
}
