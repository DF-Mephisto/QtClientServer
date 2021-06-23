#include "stdafx.h"
#include "ReceiverThread.h"

ReceiverThread::ReceiverThread(const SOCKET socket)
	: socket(socket)
{

}

void ReceiverThread::run()
{
	int received = 0;
	int expected = 0;
	QString res = "";

	while (true)
	{
		int recv_size;
		int offset = 0;
		char server_reply[10];

		if ((recv_size = recv(socket, server_reply, 9, 0)) == SOCKET_ERROR)
		{
			emit sendNotification(QString("Recv failed"), MSG_TYPE::ERROR_MSG);
			return;
		}

		if (received < 4)
		{
			int left = 4 - received;
			offset = recv_size - left > 0 ? left : recv_size;
			memcpy(&expected + received, server_reply, offset);
		}

		received += recv_size;

		if (received > 4 && offset < recv_size)
		{
			server_reply[recv_size] = '\0';
			res += QString(server_reply + offset);
		}

		if (received - 4 >= expected)
		{
			emit sendNotification(QString("Message received"), MSG_TYPE::SUCCESS_MSG);
			emit receiveMsg(QString(res));

			received = 0;
			expected = 0;
			res = "";
		}
	}
}