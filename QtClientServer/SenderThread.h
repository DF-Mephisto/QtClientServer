#pragma once

#include <QtWidgets>
#include "Constants.h"

class SenderThread : public QThread
{
	Q_OBJECT

public:
	SenderThread(const SOCKET socket);
	virtual void run() override;

private:
	const SOCKET socket;

	QString msg;
	bool newMessage;

signals:
	void sendNotification(const QString& msg, MSG_TYPE type);

private slots:
	void sendMsg(const QString& msg);

};

