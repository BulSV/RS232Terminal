#ifndef MACRO_H
#define MACRO_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QTimer>
#include <QSignalMapper>

class Macro : public QWidget
{
    Q_OBJECT
    QLineEdit *leMacro1;
    QLineEdit *leMacro2;
    QLineEdit *leMacro3;
    QLineEdit *leMacro4;
    QLineEdit *leMacro5;
    QLineEdit *leMacro6;
    QLineEdit *leMacro7;
    QLineEdit *leMacro8;
    QLineEdit *leMacro9;
    QLineEdit *leMacro10;

    QPushButton *bMacroLoad1;
    QPushButton *bMacroLoad2;
    QPushButton *bMacroLoad3;
    QPushButton *bMacroLoad4;
    QPushButton *bMacroLoad5;
    QPushButton *bMacroLoad6;
    QPushButton *bMacroLoad7;
    QPushButton *bMacroLoad8;
    QPushButton *bMacroLoad9;
    QPushButton *bMacroLoad10;

    QPushButton *bMacroSave1;
    QPushButton *bMacroSave2;
    QPushButton *bMacroSave3;
    QPushButton *bMacroSave4;
    QPushButton *bMacroSave5;
    QPushButton *bMacroSave6;
    QPushButton *bMacroSave7;
    QPushButton *bMacroSave8;
    QPushButton *bMacroSave9;
    QPushButton *bMacroSave10;

    QSpinBox *sbMacroInterval1;
    QSpinBox *sbMacroInterval2;
    QSpinBox *sbMacroInterval3;
    QSpinBox *sbMacroInterval4;
    QSpinBox *sbMacroInterval5;
    QSpinBox *sbMacroInterval6;
    QSpinBox *sbMacroInterval7;
    QSpinBox *sbMacroInterval8;
    QSpinBox *sbMacroInterval9;
    QSpinBox *sbMacroInterval10;

    QCheckBox *cbMacroActive1;
    QCheckBox *cbMacroActive2;
    QCheckBox *cbMacroActive3;
    QCheckBox *cbMacroActive4;
    QCheckBox *cbMacroActive5;
    QCheckBox *cbMacroActive6;
    QCheckBox *cbMacroActive7;
    QCheckBox *cbMacroActive8;
    QCheckBox *cbMacroActive9;
    QCheckBox *cbMacroActive10;

    QTimer tMacro;

    void view();
    void widgetInit();
    void connections();

protected:
    virtual void closeEvent(QCloseEvent *e);

private slots:
    void setPackege();

public:
    explicit Macro(QString title, QWidget *parent = 0);
};

#endif // MACRO_H
