#include "netplay_common.h"
#include <QJsonDocument>

void sendNetplayMessage(QWebSocket* webSocket, QJsonObject json)
{
    QCryptographicHash hash = QCryptographicHash(QCryptographicHash::Sha256);
    QByteArray currentTime = QByteArray::number(QDateTime::currentMSecsSinceEpoch());
    hash.addData(currentTime);
    hash.addData(QByteArray::number(NETPLAY_AUTH_CODE));
    json.insert("auth", QString(hash.result().toHex()));
    json.insert("authTime", QString(currentTime));

    QJsonDocument json_doc(json);
    webSocket->sendTextMessage(json_doc.toJson());
}
