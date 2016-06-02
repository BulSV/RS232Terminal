#ifndef HEXLINEEDIT_H
#define HEXLINEEDIT_H
#include <QLineEdit>

class HEXLineEdit : public QLineEdit
{
    Q_OBJECT
private slots:
    void changed(QString str);
public:
    HEXLineEdit(QWidget *parent = 0);
};

#endif // HEXLINEEDIT_H
