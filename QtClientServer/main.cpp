#include "stdafx.h"

#include <QtWidgets>
#include "QtClientServer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtClientServer w;
    w.show();

    return a.exec();
}
