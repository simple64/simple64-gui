#include "cheats.h"
#include "interface/core_commands.h"
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtEndian>
#include <QScrollArea>
#include <QLabel>
#include <QCheckBox>
#include <cinttypes>

CheatsDialog::CheatsDialog(QWidget *parent)
    : QDialog(parent)
{
    QString cheats_path = QDir(QCoreApplication::applicationDirPath()).filePath("cheats.json");
    QFile file(cheats_path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QJsonObject data = QJsonDocument::fromJson(file.readAll()).object();
    file.close();

    m64p_rom_header rom_header;
    (*CoreDoCommand)(M64CMD_ROM_GET_HEADER, sizeof(rom_header), &rom_header);
    QString gameName = QString("%1-%2-C:%3").arg(qFromBigEndian(rom_header.CRC1), 8, 16, QLatin1Char('0')).arg(qFromBigEndian(rom_header.CRC2), 8, 16, QLatin1Char('0')).arg(rom_header.Country_code, 2, 16).toUpper();
    QJsonObject gameData = data.value(gameName).toObject();

    QJsonDocument doc(gameData);
    QString strJson(doc.toJson(QJsonDocument::Indented));
    printf("%s\n", qPrintable(strJson));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QWidget *cheatsSettings = new QWidget(this);
    m_layout = new QGridLayout(cheatsSettings);
    

    QScrollArea *cheatsScroll = new QScrollArea(this);
    

    

    QStringList keys = gameData.keys();
    for (int i = 0; i < keys.size(); ++i)
    {
        QLabel *name = new QLabel(keys.at(i), this);
        name->setStyleSheet("padding: 10px");
        QCheckBox *box = new QCheckBox(this);
        m_layout->addWidget(name, i, 0);
        m_layout->addWidget(box, i, 1);
    }
    cheatsSettings->setLayout(m_layout);
    cheatsScroll->setMinimumWidth(cheatsSettings->sizeHint().width() + 20);
    cheatsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    cheatsScroll->setWidget(cheatsSettings);
    mainLayout->addWidget(cheatsScroll);
    setLayout(mainLayout);
}
