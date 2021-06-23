#include "stdafx.h"
#include "ServerThread.h"

ServerThread::ServerThread(QObject* parent)
	: QThread(parent), sender(nullptr), receiver(nullptr), clientSocket(INVALID_SOCKET)
{
	this->parent = parent;

    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        winsockInitialized = false;
        QMessageBox::critical(Q_NULLPTR, QString("Error"), QString("WSAStartup failed with error: %1").arg(iResult), QMessageBox::Ok);
    } else {
        winsockInitialized = true;
    }

	connect(this, SIGNAL(connected()), parent, SLOT(connected()));
	connect(this, SIGNAL(sendNotification(const QString&, MSG_TYPE)), parent, SLOT(getNotification(const QString&, MSG_TYPE)));
}

ServerThread::~ServerThread()
{
    WSACleanup();
    shutdown(clientSocket, SD_SEND);
    closesocket(clientSocket);
    delete sender;
    delete receiver;
}

void ServerThread::run()
{
    if (!winsockInitialized)
    {
        emit sendNotification("Winsock isn't initialized", MSG_TYPE::ERROR_MSG);
        return;
    }

    clientSocket = waitForConnection();
    if (clientSocket == INVALID_SOCKET)
        return;

    emit sendNotification("Server succesfully accepted new client", MSG_TYPE::SUCCESS_MSG);

    sender = new SenderThread(clientSocket);
    receiver = new ReceiverThread(clientSocket);
    connect(parent, SIGNAL(sendMsg(const QString&)), sender, SLOT(sendMsg(const QString&)));
    connect(receiver, SIGNAL(receiveMsg(const QString&)), parent, SLOT(receiveMsg(const QString&)));
    connect(sender, SIGNAL(sendNotification(const QString&, MSG_TYPE)), parent, SLOT(getNotification(const QString&, MSG_TYPE)));
    connect(receiver, SIGNAL(sendNotification(const QString&, MSG_TYPE)), parent, SLOT(getNotification(const QString&, MSG_TYPE)));

	sender->start();
	receiver->start();

	emit connected();
    exec();
}

SOCKET ServerThread::waitForConnection()
{
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct sockaddr_in server, client;
    int c;

    // Create a SOCKET for listening for clients
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        emit sendNotification(QString("socket failed with error: %1").arg(WSAGetLastError()), MSG_TYPE::ERROR_MSG);
        return INVALID_SOCKET;
    }
    
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1"); //INADDR_ANY;
    server.sin_port = htons(SERVER_PORT);
    
    //Bind
    if (bind(ListenSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        emit sendNotification(QString("bind failed with error: %1").arg(WSAGetLastError()), MSG_TYPE::ERROR_MSG);
        closesocket(ListenSocket);
        return INVALID_SOCKET;
    }

    emit sendNotification("Server is listening...", MSG_TYPE::SUCCESS_MSG);
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        emit sendNotification(QString("listen failed with error: %1").arg(WSAGetLastError()), MSG_TYPE::ERROR_MSG);
        closesocket(ListenSocket);
        return INVALID_SOCKET;
    }

    // Accept a client socket
    c = sizeof(struct sockaddr_in);
    ClientSocket = accept(ListenSocket, (struct sockaddr*)&client, &c);
    if (ClientSocket == INVALID_SOCKET) {
        emit sendNotification(QString("accept failed with error: %1").arg(WSAGetLastError()), MSG_TYPE::ERROR_MSG);
        closesocket(ListenSocket);
        return INVALID_SOCKET;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    return ClientSocket;
}