#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QString title, QWidget *parent = 0);
    ~Dialog();
};

#endif // DIALOG_H
