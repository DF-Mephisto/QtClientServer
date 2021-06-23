#include "stdafx.h"
#include "SenderThread.h"

SenderThread::SenderThread(const SOCKET socket)
	: newMessage(false), socket(socket)
{

}

void SenderThread::run()
{
	while (true)
	{
		if (newMessage)
		{
			char* data = new char[msg.length() + 4];
			int length = msg.length();
			memcpy(data, &length, 4);
			memcpy(data + 4, msg.toStdString().c_str(), length);
			if (send(socket, data, length + 4, 0) < 0)
			{
				emit sendNotification(QString("Send failed"), MSG_TYPE::ERROR_MSG);
				return;
			}
			emit sendNotification(QString("Data is sent"), MSG_TYPE::SUCCESS_MSG);

			newMessage = false;
		}

		msleep(500);
	}
}

void SenderThread::sendMsg(const QString& msg)
{
	this->msg = msg;
	newMessage = true;
}