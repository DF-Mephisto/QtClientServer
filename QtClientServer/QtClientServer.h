#pragma once
#include <WinSock2.h>
#include <QtWidgets>
#include "Constants.h"
#include "ClientThread.h"
#include "ServerThread.h"

class QtClientServer : public QWidget
{
    Q_OBJECT

public:
    QtClientServer(QWidget *parent = Q_NULLPTR);
    ~QtClientServer();

private:
    APP_TYPE appType;

    QRadioButton* clientRbtn;
    QRadioButton* serverRbtn;

    QPushButton* regBtn;
    QPushButton* sendBtn;

    QLabel* text;
    QTextEdit* textEdit;
    QLineEdit* ipEdit;

    ClientThread* clientThread;
    ServerThread* serverThread;

private slots:
    void registerApp();
    void connected();
    void getNotification(const QString& msg, MSG_TYPE type);
    void send();
    void receiveMsg(const QString& msg);

signals:
    void sendMsg(const QString& msg);
};
