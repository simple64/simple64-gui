#include "logviewer.h"

LogViewer::LogViewer()
{
    this->resize(640,480);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    area = new QScrollArea;
    mainLayout->addWidget(area);
    setLayout(mainLayout);
}

void LogViewer::showEvent(QShowEvent *event)
{
    label = new QLabel;
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setText(data);
    area->setWidget(label);
    QWidget::showEvent( event );
}

void LogViewer::addLog(QString text)
{
    data += text;
}

void LogViewer::clearLog()
{
    data.clear();
}
