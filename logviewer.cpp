#include "logviewer.h"

LogViewer::LogViewer()
{
    file = new QTemporaryFile;
    if (!file->open()) {
        delete file;
        file = nullptr;
    }

    this->resize(640,480);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    textArea = new QPlainTextEdit;
    mainLayout->addWidget(textArea);
    setLayout(mainLayout);
}

LogViewer::~LogViewer()
{
    if (file != nullptr) {
        file->close();
        delete file;
    }
}

void LogViewer::showEvent(QShowEvent *event)
{
    if (file != nullptr) file->flush();
    else return;

    qint64 pos = file->pos();
    file->seek(0);
    QTextStream in(file);
    QString data;
    while (!in.atEnd()) {
        data += in.readLine();
        data += "\n";
    }
    file->seek(pos);
    textArea->setPlainText(data);
    QWidget::showEvent( event );
}

void LogViewer::addLog(QString text)
{
    if (file == nullptr) return;

    QTextStream out(file);
    out << text;
}

void LogViewer::clearLog()
{
    if (file != nullptr) {
        file->seek(0);
        file->resize(0);
        file->flush();
    }
}
