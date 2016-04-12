#include "Dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w(QString::fromUtf8("RS232 Terminal"));
    w.show();

    return a.exec();
}
