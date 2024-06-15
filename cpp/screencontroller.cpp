#include "screencontroller.h"
#include <QtGlobal> // qDebug
#include <QtDebug> // qDebug overload

ScreenController::ScreenController(QObject* parent)
    : QObject(parent)
    , m_ptty(new Ptty(this))
    , m_historyIndex(0)
    // , m_rootHistoryIndex(0)
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

    loadBashCommandHistoryFile();
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

    //                                         -----g1----  --g2---
    static QRegularExpression promptPattern(R"(\[(.*?@.*?)\]([$#]))");
    QRegularExpressionMatch promptMatch = promptPattern.match(filteredAnsiText);
    if (promptMatch.hasMatch()) {
        // QString promptText = promptMatch.captured(1); // g1
        QString promptSymbol = promptMatch.captured(2); // g2

        // toggle root status
        bool isRoot = (promptSymbol == "#");

        // only load /root/.bash_history in root mode
        // to avoid read permission denied
        // if (isRoot) loadRootBashCommandHistoryFile();

        if (!isRoot) filteredAnsiText.replace(promptMatch.captured(0),
                                 "<span style=\"color:#cd7db7;\"><b>"
                                     + promptMatch.captured(0).toHtmlEscaped()
                                     + "</b></span>");
        else filteredAnsiText.replace(promptMatch.captured(0),
                                     "<span style=\"color:#a54242;\"><b>"
                                         + promptMatch.captured(0).toHtmlEscaped()
                                         + "</b></span>");
    }

    return filteredAnsiText;
}

QString ScreenController::removeAnsi(const QString& ansiText){
    // --------IMPROVE 1.2: enable ascii graphic programs like sl--------
    // maybe set displaymode to RichText / PlainText based on the command
    // ------------------------------------------------------------------
    QString filteredAnsiText = ansiText;
    // ignore window title escape characters
    static QRegularExpression titlePattern("\\x1b]0;.*?\\x07");
    filteredAnsiText.remove(titlePattern);

    // ignore bracketed paste escape characters
    // -------------IMPROVE 1.1: allowing bracketed paste-------------------
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

void ScreenController::commandHistoryUp() {
    // if (isRoot){
    //     if (m_rootHistoryIndex > 0){
    //         m_rootHistoryIndex--;
    //         emit showCommand(m_rootCommandHistory.at(m_rootHistoryIndex));
    //     }
    // }
    // else {
        if (m_historyIndex > 0) {
            m_historyIndex--;
            emit showCommand(m_commandHistory.at(m_historyIndex));
        }
    // }
}

void ScreenController::commandHistoryDown() {
    // if (isRoot){
    //     if (m_rootHistoryIndex < m_rootCommandHistory.size() -1) {
    //         m_rootHistoryIndex++;
    //         emit showCommand(m_rootCommandHistory.at(m_rootHistoryIndex));
    //     }
    //     else{ // at the end of root history
    //         m_rootHistoryIndex = m_rootCommandHistory.size();
    //         emit showCommand("");
    //     }
    // }
    // else {
        if (m_historyIndex < m_commandHistory.size() - 1) {
            m_historyIndex++;
            emit showCommand(m_commandHistory.at(m_historyIndex));
        } else { // at the end of history
            m_historyIndex = m_commandHistory.size();
            emit showCommand("");
        }
    // }
}

void ScreenController::logCommand(QString command){
    // if (isRoot){
    //     m_rootCommandHistory.append(command);
    //     m_rootHistoryIndex = m_rootCommandHistory.size();
    // }
    // else{
        m_commandHistory.append(command);
        m_historyIndex = m_commandHistory.size();
    // }
}

void ScreenController::loadBashCommandHistoryFile() {
    QFile filepath(QDir::homePath() + "/.bash_history");
    if (filepath.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream inputStream(&filepath);
        while (!inputStream.atEnd()) {
            m_commandHistory.append(inputStream.readLine());
        }
        filepath.close();
        m_historyIndex = m_commandHistory.size();
    } else {
        perror("Unable to open ~/.bash_history or file does not exist");
    }
}


// void ScreenController::loadRootBashCommandHistoryFile() {
//     QFile filepath("/root/.bash_history");
//     if (!filepath.exists()) {
//         qDebug() << "/root/.bash_history does not exist.";
//         return;
//     }

//     if (!filepath.isReadable()) {
//         qDebug() << "/root/.bash_history is not readable.";
//         return;
//     }

//     if (filepath.open(QIODevice::ReadOnly | QIODevice::Text)) {
//         QTextStream inputStream(&filepath);
//         while (!inputStream.atEnd()) {
//             m_rootCommandHistory.append(inputStream.readLine());
//         }
//         filepath.close();
//         m_rootHistoryIndex = m_rootCommandHistory.size();
//     } else {
//         perror("Unable to open /root/.bash_history");
//     }
// }
