#ifndef MACROSEDITING_H
#define MACROSEDITING_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QToolBar>
#include <QGridLayout>
#include <QRadioButton>
#include <QLabel>

class Macros : public QMainWindow
{
    Q_OBJECT
    QWidget *widget;
    QToolBar *toolBar;
    QLineEdit *package;
    QGridLayout *mainLay;
    QRadioButton *rbHEX;
    QRadioButton *rbDEC;
    QRadioButton *rbASCII;
    QLabel *lbHEX;
    QLabel *lbDEC;
    QLabel *lbASCII;

public:
    explicit Macros(QWidget *parent = 0);

signals:
    void packageChanged(QString);

public slots:
    void saveDialog();
    void openDialog();
};

#endif // MACROSEDITING_H
