#ifndef MACROSEDITING_H
#define MACROSEDITING_H

#include <QWidget>
#include "Macros.h"
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>

class MacrosEditing : public QWidget
{
    Q_OBJECT
    QPushButton *load;
    QPushButton *save;
    QLineEdit *package;
    QSpinBox *interval;
    QHBoxLayout *mainLay;

public:
    explicit MacrosEditing(QString title, QString p, int i, QWidget *parent = 0);

signals:

private slots:
    void saveDialog();
};

#endif // MACROSEDITING_H
