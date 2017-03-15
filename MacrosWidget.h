#ifndef MACROS_WIDGET_H
#define MACROS_WIDGET_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QToolBar>
#include <QGridLayout>
#include <QRadioButton>
#include <QLabel>
#include <QAction>
#include <QSettings>

class MacrosWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit MacrosWidget(QWidget *parent = 0);

    void update(int time);
    void saveSettings(QSettings *settings, int macrosIndex);
    void loadSettings(QSettings *settings, int macrosIndex);
signals:
    void packageChanged(const QString &package);
    void upd(bool, const QString&, int);
    void act(bool);
public slots:
    void hexChecked();
    void asciiChecked();
    void decChecked();
    void compute(const QString &str);
    void reset();
    void saveAs();
    void save();
    void openDialog();
    bool openPath(const QString &fileName);
private:
    QWidget *centralWidget;
    QToolBar *toolBar;
    QGridLayout *mainLayout;
    QLineEdit *lbHEX;
    QLineEdit *lbDEC;
    QLineEdit *lbASCII;
    QLineEdit *package;
    QRadioButton *rbHEX;
    QRadioButton *rbDEC;
    QRadioButton *rbASCII;
    QAction *aCR;
    QAction *aLF;

    bool isFromFile;
    QString path;

    int time;

    void saveToFile(const QString &path);
    void openFile(const QString &path);
    void connections();
    void view();
};

#endif // MACROS_WIDGET_H
