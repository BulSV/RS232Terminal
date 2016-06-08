#ifndef MACROSEDITING_H
#define MACROSEDITING_H

#include <QWidget>
#include "Macros.h"

class MacrosEditing : public QWidget
{
    Q_OBJECT
public:
    explicit MacrosEditing(Macros *m, QWidget *parent = 0);

signals:

public slots:
};

#endif // MACROSEDITING_H
