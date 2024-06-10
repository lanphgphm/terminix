#include "screencontroller.h"
#include <QtGlobal> // qDebug
#include <QRegularExpression>
#include <QMap>

ScreenController::ScreenController(QObject* parent)
    : QObject(parent)
{
    // constructor
}

ScreenController::~ScreenController() {
    // destructor
}

void ScreenController::setPtty(Ptty* ptty){
    m_ptty = ptty;
}

void ScreenController::commandReceivedFromView(QString command){
    emit commandReadySendToPty(command);
}

void ScreenController::resultReceivedFromPty(QString result){
    // QString htmlResult = ansiToHtml(result);
    // emit resultReadySendToView(htmlResult);

    QString strippedResult = ansiToPlainText(result);
    emit resultReadySendToView(strippedResult);

    // emit resultReadySendToView(result); // baseline ugly
}

QString ScreenController::ansiToHtml(const QString ansiText){
    // devil's formatting language: https://en.wikipedia.org/wiki/ANSI_escape_code
    // INCOMPLETE CODE. WILL BREAK `pstree`
    QString htmlText = ansiText;
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

    QRegularExpression pattern("\\x1b\\[([0-9;]+)m"); // also try "\\x1b\\[[0-9;]*m"
    auto i = pattern.globalMatch(ansiText);
    int lastPosition = 0;

    while(i.hasNext()){
        auto match = i.next();
        QString code = match.captured(1);
        QStringList codeList = code.split(';');


        htmlText += ansiText.mid(lastPosition, match.capturedStart() - lastPosition);
        bool reset = false;
        QString htmlStartTag, htmlEndTag;

        for (const QString code: codeList){
            int num = code.toInt();
            if (num==0) reset = true;
            else if (num==1){
                htmlStartTag.append("<b>");
                htmlEndTag.prepend("</b>");
            }
            else if (num==3){
                htmlStartTag.append("<i>");
                htmlEndTag.prepend("</i>");
            }
            else if (colorMap.contains(num)){
                htmlStartTag.append("<span style=\"color:" + colorMap[num] + "\">");
                htmlEndTag.prepend("</span>");
            }
        }

        if (reset) {
            htmlText.append("</span></b></i>");
            htmlStartTag.clear();
        }
        htmlText += htmlStartTag;
        lastPosition = match.capturedEnd();
    }
    htmlText += ansiText.mid(lastPosition);

    return htmlText;
}

QString ScreenController::ansiToPlainText(const QString ansiText){
    static QRegularExpression pattern("\\x1b\\[[0-9;]*m");
    QString plainText = ansiText;
    plainText = plainText.replace(pattern, "");
    return plainText;
}
