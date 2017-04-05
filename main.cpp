#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
#if defined (Q_OS_UNIX)
    app.setWindowIcon(QIcon(":/Resources/Terminal.png"));
#endif
    MainWindow window(QObject::tr("Terminal"));
    window.show();

    return app.exec();
}
