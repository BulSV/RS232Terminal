#include "Dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Dialog window(QString::fromUtf8("RS232 Terminal"));
    window.show();

    return app.exec();
}
