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
    QLineEdit *lbHEX;
    QLineEdit *lbDEC;
    QLineEdit *lbASCII;

public:
    explicit Macros(QWidget *parent = 0);
    void update(int t);

signals:
    void packageChanged(QString);
    void upd(bool, QString, int);

public slots:
    void rbChecked() { compute(package->text()); }
    void compute(QString str);
    void reset();
    void saveAs();
    void save();
    void openDialog();

private:
    int time;
    QString path;
};

#endif // MACROSEDITING_H
