#ifndef CLICKABLE_LABEL_H
#define CLICKABLE_LABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget * parent = 0);
    explicit ClickableLabel(const QString &text, QWidget * parent = 0);
    void setCheckable(bool checkable);
    void setChecked(bool checked);
    bool isChecked() const;
signals:
    void clicked(bool checked = false);
    void rightClicked();
    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
private:
    bool checkable;
    bool checked;
};

#endif // CLICKABLE_LABEL_H
