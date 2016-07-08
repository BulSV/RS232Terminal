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
    public:
    QWidget *widget;
    QToolBar *toolBar;
    QGridLayout *mainLay;
    QLineEdit *lbHEX;
    QLineEdit *lbDEC;
    QLineEdit *lbASCII;
    QLineEdit *package;
    QRadioButton *rbHEX;
    QRadioButton *rbDEC;
    QRadioButton *rbASCII;

    explicit Macros(QWidget *parent = 0);
    void update(int t);
    bool isFromFile;
    QString path;

signals:
    void packageChanged(QString);
    void upd(bool, QString, int);
    void act(bool);

public slots:
    void hexChecked() { package->setText(lbHEX->text()); }
    void asciiChecked() { package->setText(lbASCII->text()); }
    void decChecked() { package->setText(lbDEC->text()); }
    void compute(QString str);
    void reset();
    void saveAs();
    void save();
    void openDialog();
    bool openPath(QString fileName);

private:
    int time;
};

#endif // MACROSEDITING_H
