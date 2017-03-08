#ifndef RIGHT_CLICKED_BUTTON
#define RIGHT_CLICKED_BUTTON

#include <QPushButton>
#include <QMouseEvent>

class RightClickedButton : public QPushButton
{
    Q_OBJECT
public:
    explicit RightClickedButton(QString title, QWidget *parent = 0);
protected slots:
    virtual void mousePressEvent(QMouseEvent *e);
signals:
    void rightClicked();
};

#endif // RIGHT_CLICKED_BUTTON
