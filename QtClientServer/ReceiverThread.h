#pragma once

#include <QtWidgets>
#include "Constants.h"

class ReceiverThread : public QThread
{
	Q_OBJECT

public:
	ReceiverThread(const SOCKET socket);
	virtual void run() override;

private:
	const SOCKET socket;

signals:
	void receiveMsg(const QString& msg);
	void sendNotification(const QString& msg, MSG_TYPE type);

};