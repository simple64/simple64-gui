#include "../mainwindow.h"
#include "../cheats.h"
#include "createroom.h"
#include "waitroom.h"
#include "../version.h"
#include "../interface/core_commands.h"
#include "netplay_common.h"
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QInputDialog>
#include <QLabel>

CreateRoom::CreateRoom(QWidget *parent)
    : QDialog(parent)
{
    QGridLayout *layout = new QGridLayout(this);

    QRegularExpression rx("[a-zA-Z0-9]+");
    QValidator *validator = new QRegularExpressionValidator(rx, this);

    QLabel *nameLabel = new QLabel("Room Name", this);
    layout->addWidget(nameLabel, 0, 0);
    nameEdit = new QLineEdit(this);
    nameEdit->setValidator(validator);
    nameEdit->setMaxLength(30);
    layout->addWidget(nameEdit, 0, 1);

    QLabel *passwordLabel = new QLabel("Password (Optional)", this);
    layout->addWidget(passwordLabel, 1, 0);
    passwordEdit = new QLineEdit(this);
    layout->addWidget(passwordEdit, 1, 1);

    QLabel *romLabel = new QLabel("ROM", this);
    layout->addWidget(romLabel, 2, 0);
    romButton = new QPushButton("ROM Path", this);
    connect(romButton, &QPushButton::released, this, &CreateRoom::handleRomButton);
    layout->addWidget(romButton, 2, 1);

    QLabel *playerNameLabel = new QLabel("Player Name", this);
    layout->addWidget(playerNameLabel, 3, 0);
    playerNameEdit = new QLineEdit(this);
    playerNameEdit->setValidator(validator);
    playerNameEdit->setMaxLength(30);
    if (w->getSettings()->contains("netplay_name"))
        playerNameEdit->setText(w->getSettings()->value("netplay_name").toString());
    layout->addWidget(playerNameEdit, 3, 1);

    QLabel *serverLabel = new QLabel("Server", this);
    layout->addWidget(serverLabel, 6, 0);
    serverChooser = new QComboBox(this);
    serverChooser->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    layout->addWidget(serverChooser, 6, 1);
    connect(serverChooser, &QComboBox::currentIndexChanged, this, &CreateRoom::handleServerChanged);

    QLabel *pingLabel = new QLabel("Your ping:", this);
    layout->addWidget(pingLabel, 7, 0);
    pingValue = new QLabel(this);
    pingValue->setText("(Calculating)");
    layout->addWidget(pingValue, 7, 1);

    QFrame *lineH1 = new QFrame(this);
    lineH1->setFrameShape(QFrame::HLine);
    lineH1->setFrameShadow(QFrame::Sunken);
    layout->addWidget(lineH1, 8, 0, 1, 2);

    createButton = new QPushButton("Create Game", this);
    connect(createButton, &QPushButton::released, this, &CreateRoom::handleCreateButton);
    layout->addWidget(createButton, 9, 0, 1, 2);

    setLayout(layout);

    connect(&manager, &QNetworkAccessManager::finished, this,
            &CreateRoom::downloadFinished);

    connect(this, &CreateRoom::finished, this, &CreateRoom::onFinished);

    QNetworkRequest request(QUrl(QStringLiteral("https://m64p.s3.amazonaws.com/servers.json")));
    manager.get(request);

    broadcastSocket.bind(QHostAddress(QHostAddress::AnyIPv4), 0);
    connect(&broadcastSocket, &QUdpSocket::readyRead, this, &CreateRoom::processBroadcast);
    QByteArray multirequest;
    multirequest.append(1);
    broadcastSocket.writeDatagram(multirequest, QHostAddress::Broadcast, 45000);

    launched = 0;
}

void CreateRoom::processBroadcast()
{
    while (broadcastSocket.hasPendingDatagrams())
    {
        QNetworkDatagram datagram = broadcastSocket.receiveDatagram();
        QByteArray incomingData = datagram.data();
        QJsonDocument json_doc = QJsonDocument::fromJson(incomingData);
        QJsonObject json = json_doc.object();
        QStringList servers = json.keys();
        for (int i = 0; i < servers.size(); ++i)
            serverChooser->addItem(servers.at(i), json.value(servers.at(i)).toString());
    }
}

void CreateRoom::onFinished(int)
{
    broadcastSocket.close();
    (*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);
    if (!launched && webSocket)
    {
        webSocket->close();
        webSocket->deleteLater();
    }
}

void CreateRoom::handleRomButton()
{
    filename = QFileDialog::getOpenFileName(this,
                                            tr("Open ROM"), w->getSettings()->value("ROMdir").toString(), tr("ROM Files (*.n64 *.N64 *.z64 *.Z64 *.v64 *.V64 *.rom *.ROM *.zip *.ZIP *.7z)"));
    if (!filename.isNull())
    {
        romButton->setText(filename);
    }
}

void CreateRoom::handleCreateButton()
{
    QMessageBox msgBox;
    if (serverChooser->currentData() == "Custom" && customServerHost.isEmpty())
    {
        msgBox.setText("Custom Server Address is invalid");
        msgBox.exec();
        return;
    }
    if (loadROM(filename) == M64ERR_SUCCESS)
    {
        createButton->setEnabled(false);
        (*CoreDoCommand)(M64CMD_ROM_GET_SETTINGS, sizeof(rom_settings), &rom_settings);

        connectionTimer = new QTimer(this);
        connectionTimer->setSingleShot(true);
        connectionTimer->start(5000);
        connect(connectionTimer, &QTimer::timeout, this, &CreateRoom::connectionFailed);
        connect(webSocket, &QWebSocket::disconnected, connectionTimer, &QTimer::stop);
        connect(webSocket, &QObject::destroyed, connectionTimer, &QTimer::stop);

        if (webSocket->isValid())
        {
            createRoom();
        }
        else
        {
            connect(webSocket, &QWebSocket::connected, this, &CreateRoom::createRoom);
        }
    }
    else
    {
        msgBox.setText("Could not open ROM");
        msgBox.exec();
    }
}

void CreateRoom::createRoom()
{
    connectionTimer->stop();
    QJsonObject json;
    json.insert("type", "request_create_room");
    json.insert("room_name", nameEdit->text());
    json.insert("player_name", playerNameEdit->text());
    json.insert("password", passwordEdit->text());
    json.insert("MD5", QString(rom_settings.MD5));
    json.insert("game_name", QString(rom_settings.goodname));
    json.insert("client_sha", QStringLiteral(GUI_VERSION));
    json.insert("netplay_version", NETPLAY_VER);

    QString gameName = getCheatGameName();
    QJsonObject gameData = loadCheatData(gameName);
    QJsonObject cheats = getCheatsFromSettings(gameName, gameData);
    if (!cheats.isEmpty())
    {
        QJsonDocument cheatsDoc(cheats);
        QJsonObject features;
        features.insert("cheats", QString(cheatsDoc.toJson(QJsonDocument::Compact)));
        json.insert("features", features);
    }

    addAuthData(webSocket, &json);

    QJsonDocument json_doc(json);
    webSocket->sendTextMessage(json_doc.toJson());
}

void CreateRoom::processTextMessage(QString message)
{
    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setTextInteractionFlags(Qt::TextBrowserInteraction);
    QJsonDocument json_doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject json = json_doc.object();
    if (json.value("type").toString() == "reply_create_room")
    {
        if (json.value("accept").toInt() == 0)
        {
            json.remove("type");
            launched = 1;
            WaitRoom *waitRoom = new WaitRoom(filename, json, webSocket, parentWidget());
            waitRoom->show();
            accept();
        }
        else
        {
            (*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);
            msgBox.setText(json.value("message").toString());
            msgBox.exec();
            createButton->setEnabled(true);
        }
    }
}

void CreateRoom::downloadFinished(QNetworkReply *reply)
{
    if (!reply->error())
    {
        QJsonDocument json_doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject json = json_doc.object();
        QStringList servers = json.keys();
        for (int i = 0; i < servers.size(); ++i)
            serverChooser->addItem(servers.at(i), json.value(servers.at(i)).toString());
        serverChooser->addItem(QString("Custom"), QString("Custom"));
    }

    reply->deleteLater();
}

void CreateRoom::handleServerChanged(int index)
{
    if (serverChooser->itemData(index) == "Custom")
    {
        bool ok;
        QString host = QInputDialog::getText(this, "Custom Netplay Server", "IP Address / Host:", QLineEdit::Normal, "", &ok);

        if (ok && !host.isEmpty())
        {
            customServerHost = host;
        }
    }

    if (webSocket)
    {
        webSocket->close();
        webSocket->deleteLater();
    }

    pingValue->setText("(Calculating)");

    webSocket = new QWebSocket;
    connect(webSocket, &QWebSocket::pong, this, &CreateRoom::updatePing);
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CreateRoom::sendPing);
    connect(webSocket, &QWebSocket::disconnected, timer, &QTimer::stop);
    connect(webSocket, &QObject::destroyed, timer, &QTimer::stop);

    connect(webSocket, &QWebSocket::textMessageReceived, this, &CreateRoom::processTextMessage);

    timer->start(2500);
    QString serverAddress = serverChooser->itemData(index) == "Custom" ? customServerHost.prepend("ws://") : serverChooser->itemData(index).toString();
    QUrl serverUrl = QUrl(serverAddress);
    if (serverChooser->itemData(index) == "Custom" && serverUrl.port() < 0)
        // Be forgiving of custom server addresses that forget the port
        serverUrl.setPort(45000);

    webSocket->open(serverUrl);
}

void CreateRoom::connectionFailed()
{
    (*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);
    QMessageBox msgBox;
    msgBox.setText("Could not connect to netplay server.");
    msgBox.exec();
    // Allow them to try again
    createButton->setEnabled(true);
    // Trigger input dialog for custom IP address, if using custom IP
    handleServerChanged(serverChooser->currentIndex());
}

void CreateRoom::updatePing(quint64 elapsedTime, const QByteArray &)
{
    pingValue->setText(QString::number(elapsedTime) + " ms");
}

void CreateRoom::sendPing()
{
    webSocket->ping();
}
