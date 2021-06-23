#include "stdafx.h"
#include "QtClientServer.h"

QtClientServer::QtClientServer(QWidget *parent)
    : QWidget(parent), appType(APP_TYPE::CLIENT_TYPE), 
    clientThread(nullptr), serverThread(nullptr)
{
    resize(500, 400);

    clientRbtn = new QRadioButton("Client");
    serverRbtn = new QRadioButton("Server");
    clientRbtn->setChecked(true);

    regBtn = new QPushButton("Register");
    sendBtn = new QPushButton("Send");

    ipEdit = new QLineEdit;
    ipEdit->setText("127.0.0.1");
    QLabel* ipLabel = new QLabel("IP: ");

    textEdit = new QTextEdit;

    QScrollArea* sa = new QScrollArea;
    sa->setWidgetResizable(true);
    text = new QLabel;
    sa->setWidget(text);
    text->setAlignment(Qt::AlignTop);

    sendBtn->setEnabled(false);
    textEdit->setEnabled(false);

    QHBoxLayout* typeLayout = new QHBoxLayout;
    typeLayout->addWidget(clientRbtn);
    typeLayout->addWidget(serverRbtn);
    typeLayout->addWidget(regBtn);

    QHBoxLayout* ipLayout = new QHBoxLayout;
    ipLayout->addWidget(ipLabel);
    ipLayout->addWidget(ipEdit);

    QHBoxLayout* msgLayout = new QHBoxLayout;
    msgLayout->addWidget(textEdit);
    msgLayout->addWidget(sendBtn);
    
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(typeLayout);
    mainLayout->addLayout(ipLayout);
    mainLayout->addLayout(msgLayout);
    mainLayout->addWidget(sa);

    setLayout(mainLayout);

    connect(clientRbtn, &QRadioButton::toggled, [&](bool checked) {if (checked) appType = APP_TYPE::CLIENT_TYPE;
        ipEdit->setEnabled(true); });
    connect(serverRbtn, &QRadioButton::toggled, [&](bool checked) {if (checked) appType = APP_TYPE::SERVER_TYPE; 
        ipEdit->setEnabled(false); });
    connect(regBtn, SIGNAL(clicked()), SLOT(registerApp()));
    connect(sendBtn, SIGNAL(clicked()), SLOT(send()));

    qRegisterMetaType<MSG_TYPE>("MSG_TYPE");
}

QtClientServer::~QtClientServer()
{
    delete clientThread;
    delete serverThread;
}

void QtClientServer::registerApp()
{
    switch (appType)
    {
    case CLIENT_TYPE:
    {
        clientThread = new ClientThread(ipEdit->text(), this);
        clientThread->start();
        break;
    }
    case SERVER_TYPE:
    {
        serverThread = new ServerThread(this);
        serverThread->start();
        break;
    }
    }
}

void QtClientServer::getNotification(const QString& msg, MSG_TYPE type)
{
    QString color;

    switch (type)
    {
    case MSG_TYPE::SUCCESS_MSG:
    {
        color = "green";
        break;
    }
    case MSG_TYPE::ERROR_MSG:
    {
        color = "red";
        break;
    }
    }

    QString newText = "<div style=\"color: " + color + ";\">" + msg + "</div>";
    text->setText(text->text() + newText);
}

void QtClientServer::connected()
{
    sendBtn->setEnabled(true);
    textEdit->setEnabled(true);

    regBtn->setEnabled(false);
    clientRbtn->setEnabled(false);
    serverRbtn->setEnabled(false);
}

void QtClientServer::send()
{
    int textLength = textEdit->toPlainText().length();
    emit sendMsg(textEdit->toPlainText());
}

void QtClientServer::receiveMsg(const QString& msg)
{
    text->setText(text->text() + "<br/>" + msg);
}