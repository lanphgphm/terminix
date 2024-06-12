#include "screencontroller.h"
#include <QtGlobal> // qDebug
#include <QRegularExpression>
#include <QMap>

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
    QString htmlResult = ansiToHtml(result);
    emit resultReadySendToView(htmlResult);
}

QString ScreenController::ansiToHtml(const QString& ansiText){
    // Devil's formatting language: https://en.wikipedia.org/wiki/ANSI_escape_code
    QString htmlText = "<pre>"; // preserve whitespace
    QMap<int, QString> colorMap = {
        {30, "black"},
        {31, "red"},
        {32, "green"},
        {33, "yellow"},
        {34, "blue"},
        {35, "magenta"},
        {36, "cyan"},
        {37, "white"}
    };

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
