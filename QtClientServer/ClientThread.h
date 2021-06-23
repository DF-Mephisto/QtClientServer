#pragma once

#include <QtWidgets>

#include "Constants.h"
#include "SenderThread.h"
#include "ReceiverThread.h"

class ClientThread : public QThread
{
	Q_OBJECT

public:
	ClientThread(QString ip, QObject* parent);
	~ClientThread();
	virtual void run() override;

private:
	QObject* parent;
	SenderThread* sender;
	ReceiverThread* receiver;

	QString ip;
	WSADATA wsaData;
	SOCKET serverSocket;
	bool winsockInitialized;

	SOCKET waitForConnection();

signals:
	void connected();
	void sendNotification(const QString& msg, MSG_TYPE type);

};

