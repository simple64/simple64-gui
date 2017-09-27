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
    area = new QScrollArea;
    mainLayout->addWidget(area);
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

    label = new QLabel;
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    qint64 pos = file->pos();
    file->seek(0);
    QTextStream in(file);
    QString data;
    while (!in.atEnd()) {
        data += in.readLine();
        data += "\n";
    }
    file->seek(pos);
    label->setText(data);
    area->setWidget(label);
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
