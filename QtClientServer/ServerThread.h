#pragma once

#include <QtWidgets>

#include "Constants.h"
#include "SenderThread.h"
#include "ReceiverThread.h"

class ServerThread : public QThread
{
	Q_OBJECT

public:
	ServerThread(QObject* parent);
	~ServerThread();
	virtual void run() override;
private:
	QObject* parent;
	SenderThread* sender;
	ReceiverThread* receiver;

	WSADATA wsaData;
	SOCKET clientSocket;
	bool winsockInitialized;

	SOCKET waitForConnection();

signals:
	void connected();
	void sendNotification(const QString& msg, MSG_TYPE type);

};