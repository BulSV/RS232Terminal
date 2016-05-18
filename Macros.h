#ifndef MACROS_H
#define MACROS_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QGridLayout>

class Macros : public QWidget
{
    Q_OBJECT

    void connections();

private slots:
    void singleSend();
    void Delete();
    void save();
    void stateChange(bool check);
    void textChanged(QString text);
    void openLoad();

signals:
    DeleteMacros(int);
    WriteMacros(const QString &str);
    Sending(bool);

public:
    QLineEdit *leMacros;
    QPushButton *bMacrosLoad;
    QPushButton *bMacrosSave;
    QSpinBox *sbMacrosInterval;
    QCheckBox *cbMacrosActive;
    QPushButton *bMacros;
    QPushButton *bMacrosDel;

    int index;
    QString path;
    bool isFromFile;

    bool openPath(QString);
    explicit Macros(int id, QString p, bool buttonActive, QWidget *parent = 0);
};

#endif // MACROS_H
