#ifndef MYPUSHBUTTON
#define MYPUSHBUTTON
#include <QPushButton>
#include <QMouseEvent>

class MyPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit MyPushButton(QString title, QWidget *parent = 0)
    {
        setParent(parent);
        setText(title);
    }

private slots:
    void mousePressEvent(QMouseEvent *e)
    {
        if (e->button() == Qt::RightButton)
            emit rightClicked();
        if (e->button() == Qt::LeftButton)
            click();
    }

signals:
    void rightClicked();
};

#endif // MYPUSHBUTTON

