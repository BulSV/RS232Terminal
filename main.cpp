#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
#if defined (Q_OS_UNIX)
    app.setWindowIcon(QIcon(":/Resources/terminal.png"));
#endif
    MainWindow window(QObject::tr("Terminal"));
    window.show();

    return app.exec();
}
