#pragma once
#include <QtCore>

#define DEFAULT_BUFLEN 512
#define SERVER_PORT 8080
#define CLIENT_PORT 8081

enum APP_TYPE
{
	CLIENT_TYPE, SERVER_TYPE
};

enum MSG_TYPE
{
	SUCCESS_MSG, ERROR_MSG
};
Q_DECLARE_METATYPE(MSG_TYPE)