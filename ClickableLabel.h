#ifndef CLICKABLE_LABEL_H
#define CLICKABLE_LABEL_H

#include <QLabel>
#include <QIcon>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget * parent = 0);
    explicit ClickableLabel(const QString &text, QWidget * parent = 0);
    void setCheckable(bool checkable);
signals:
    void clicked(bool checked = false);
    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);
private:
    bool checkable;
};

#endif // CLICKABLE_LABEL_H
