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

void ScreenController::handleControlKeyPress(Qt::Key key){
    if (keySignalMap.contains(key)) {
        int signal = keySignalMap[key];
        m_ptty->sendSignal(signal);
    }
}

QString ScreenController::processPrompt(const QString& ansiText){
    // Processing prompt
    QString filteredAnsiText = ansiText;
    static QRegularExpression titlePattern("\\x1b]0;(.*?\\x07)");
    filteredAnsiText.remove(titlePattern);

    // begin bracketed paste \x1b[?2004h; end bracketed paste \x1b[?2004l
    static QRegularExpression bracketedPastePattern("\\x1b\\[\\?2004[h|l]");
    filteredAnsiText.remove(bracketedPastePattern);

    // Regular expression to match the user prompt
    static QRegularExpression promptPattern(R"((\[.*?@.*?\]\$))");
    QRegularExpressionMatch promptMatch = promptPattern.match(filteredAnsiText);
    if (promptMatch.hasMatch()) {
        QString promptText = promptMatch.captured(1);
        filteredAnsiText.replace(promptText,
                                 "<span style=\"color:#cd7db7;\"><b>"
                                     + promptText.toHtmlEscaped()
                                     + "</b></span>");
    }

    //-------------IMPROVE 0.4: color root prompt differently----------------
    //----------------------------------------------------------------------

    return filteredAnsiText;
}

QString removeAnsi(const QString& ansiTest){
    // --------IMPROVE 0.3: enable ascii graphic programs like sl--------
    // maybe set displaymode to RichText / PlainText based on the command
    // ------------------------------------------------------------------
    QString filteredAnsiText = ansiText;
    // Remove window title sequences
    static QRegularExpression titlePattern("\\x1b]0;.*?\\x07");
    filteredAnsiText.remove(titlePattern);

    // Remove bracketed paste sequences
    // -------------IMPROVE 0.2: allowing bracketed paste-------------------
    static QRegularExpression bracketedPastePattern("\\x1b\\[\\?2004[hl]");
    filteredAnsiText.remove(bracketedPastePattern);
    // ---------------------------------------------------------------------

    return filteredAnsiText;
}

QString ScreenController::ansiToHtml(const QString& ansiText) {
    QString htmlText = "<pre>"; // Preserve whitespace
    QString filteredAnsiText = removeAnsi(ansiText);
    filteredAnsiText = processPrompt(ansiText);


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

        if (command == "m") {
            QStringList codeList = parameters.split(';');
            bool reset = false;

            for (const QString& code : codeList) {
                int num = code.toInt();
                if (num == 0) { // Reset all formatting
                    reset = true;
                } else if (num == 1) {
                    currentStyle += "font-weight:bold;";
                } else if (colorMap.contains(num)) {
                    currentStyle += "color:" + colorMap[num] + ";";
                }
            }

            if (reset) {
                while (!openTags.isEmpty()) {
                    htmlText += openTags.takeLast();
                }
                currentStyle.clear();
            }

            if (!currentStyle.isEmpty()) {
                htmlText += "<span style=\"" + currentStyle + "\">";
                openTags.append("</span>");
            }
        }

        lastPosition = match.capturedEnd();
    }

    htmlText += view.mid(lastPosition).toString();
    while (!openTags.isEmpty()) {
        htmlText += openTags.takeLast();
    }
    htmlText += "</pre>";

    // htmlText.replace("</pre><pre>", ""); // not working because this pair only exist in the already rendered output

    return htmlText;
}
