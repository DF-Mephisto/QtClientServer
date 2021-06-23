#include "stdafx.h"
#include "ClientThread.h"

ClientThread::ClientThread(QString ip, QObject* parent)
	: ip(ip), QThread(parent), sender(nullptr), receiver(nullptr), serverSocket(INVALID_SOCKET)
{
	this->parent = parent;

	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		winsockInitialized = false;
		QMessageBox::critical(Q_NULLPTR, QString("Error"), QString("WSAStartup failed with error: %1").arg(iResult), QMessageBox::Ok);
	}
	else {
		winsockInitialized = true;
	}

	connect(this, SIGNAL(connected()), parent, SLOT(connected()));
	connect(this, SIGNAL(sendNotification(const QString&, MSG_TYPE)), parent, SLOT(getNotification(const QString&, MSG_TYPE)));
}

ClientThread::~ClientThread()
{
	WSACleanup();
	shutdown(serverSocket, SD_SEND);
	closesocket(serverSocket);
	delete sender;
	delete receiver;
}

void ClientThread::run()
{
	if (!winsockInitialized)
	{
		emit sendNotification("Winsock isn't initialized", MSG_TYPE::ERROR_MSG);
		return;
	}

	serverSocket = waitForConnection();
	if (serverSocket == INVALID_SOCKET)
		return;

	emit sendNotification("Successfully connected to server", MSG_TYPE::SUCCESS_MSG);

	sender = new SenderThread(serverSocket);
	receiver = new ReceiverThread(serverSocket);
	connect(parent, SIGNAL(sendMsg(const QString&)), sender, SLOT(sendMsg(const QString&)));
	connect(receiver, SIGNAL(receiveMsg(const QString&)), parent, SLOT(receiveMsg(const QString&)));
	connect(sender, SIGNAL(sendNotification(const QString&, MSG_TYPE)), parent, SLOT(getNotification(const QString&, MSG_TYPE)));
	connect(receiver, SIGNAL(sendNotification(const QString&, MSG_TYPE)), parent, SLOT(getNotification(const QString&, MSG_TYPE)));

	sender->start();
	receiver->start();

	emit connected();
	exec();
}

SOCKET ClientThread::waitForConnection()
{
	SOCKET ServerSocket = INVALID_SOCKET;

	struct sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ip.toStdString().c_str()); //INADDR_ANY;
	server.sin_port = htons(SERVER_PORT);

	while (true) {

		// Create a SOCKET for connecting to server
		ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ServerSocket == INVALID_SOCKET) {
			emit sendNotification(QString("socket failed with error: %1").arg(WSAGetLastError()), MSG_TYPE::ERROR_MSG);
			return INVALID_SOCKET;
		}

		emit sendNotification(QString("connecting to the server"), MSG_TYPE::SUCCESS_MSG);

		// Connect to server.
		if (::connect(ServerSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
		{
			emit sendNotification(QString("socket failed to connect: %1").arg(WSAGetLastError()), MSG_TYPE::ERROR_MSG);
			closesocket(ServerSocket);
			continue;
		}

		break;
	}
	
	return ServerSocket;
}