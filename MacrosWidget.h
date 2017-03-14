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

class MacrosWidget : public QMainWindow
{
    Q_OBJECT
public:
    QLineEdit *package;
    QRadioButton *rbHEX;
    QRadioButton *rbDEC;
    QRadioButton *rbASCII;

    explicit MacrosWidget(QWidget *parent = 0);
    void update(unsigned short int t);
    bool isFromFile;
    QString path;
signals:
    void packageChanged(QString);
    void upd(bool, QString, int);
    void act(bool);
public slots:
    void hexChecked();
    void asciiChecked();
    void decChecked();
    void compute(QString str);
    void reset();
    void saveAs();
    void save();
    void openDialog();
    bool openPath(QString fileName);
private:
    QWidget *centralWidget;
    QToolBar *toolBar;
    QGridLayout *mainLayout;
    QLineEdit *lbHEX;
    QLineEdit *lbDEC;
    QLineEdit *lbASCII;
    QAction *aCR;
    QAction *aLF;

    int time;

    void saveToFile(const QString &path);
    void openFile(const QString &path);
    void connections();
    void view();
};

#endif // MACROS_WIDGET_H
