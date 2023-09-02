#include "cheats.h"
#include "interface/core_commands.h"
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtEndian>
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
}
