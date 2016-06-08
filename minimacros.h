#ifndef MINIMACROS
#define MINIMACROS

#include <QHBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QMouseEvent>

class MyPushButton : public QPushButton
{
    Q_OBJECT

public:
    QRightClickButton(QWidget *parent = 0);

private slots:
    void mousePressEvent(QMouseEvent *e)
    {
        if(e->button()==Qt::RightButton)
                emit rightClicked();
    }

signals:
    void rightClicked();
};

class MiniMacros : public QWidget
{
    Q_OBJECT
signals:
    bPress(int index);
    cbCheckInterval(int index, bool check);
    cbCheckPeriod(int index, bool check);
    editMacros(int index);

public:
    int index;
    QHBoxLayout *layout;
    QCheckBox *cbMiniMacrosInterval;
    QCheckBox *cbMiniMacrosPeriod;
    MyPushButton *bMiniMacros;

    MiniMacros(int i, QString title, QWidget *parent = 0) : QWidget(parent)
    {
        index = i;
        cbMiniMacrosInterval = new QCheckBox;
        cbMiniMacrosPeriod = new QCheckBox;
        bMiniMacros = new MyPushButton;
        layout = new QHBoxLayout;
        bMiniMacros->setText(title);
        bMiniMacros->setStyleSheet("font-weight: bold;");

        layout->setSpacing(2);
        layout->setMargin(2);

        layout->addWidget(cbMiniMacrosInterval);
        layout->addWidget(cbMiniMacrosPeriod);
        layout->addWidget(bMiniMacros);
        setLayout(layout);

        connect(bMiniMacros, SIGNAL(clicked(bool)), this, SLOT(click()));
        connect(bMiniMacros, SIGNAL(rightClicked()), this, SLOT(rightClick()));
        connect(cbMiniMacrosInterval, SIGNAL(toggled(bool)), this, SLOT(ckeckInterval(bool)));
        connect(cbMiniMacrosPeriod, SIGNAL(toggled(bool)), this, SLOT(ckeckPeriod(bool)));
    }

public slots:
    void click() { emit bPress(index); }
    void rightClick() { emit editMacros(index); }
    void ckeckInterval(bool check) {
        emit cbCheckInterval(index, check);
        if (check)
        {
            cbMiniMacrosPeriod->setEnabled(false);
            bMiniMacros->setStyleSheet("font-weight: bold; color: red;");
        }
        else
        {
            cbMiniMacrosPeriod->setEnabled(true);
            bMiniMacros->setStyleSheet("font-weight: bold;");
        }
    }

    void ckeckPeriod(bool check) {
        emit cbCheckPeriod(index, check);
        if (check)
        {
            cbMiniMacrosInterval->setEnabled(false);
            bMiniMacros->setStyleSheet("font-weight: bold; color: green;");
        }
        else
        {
            cbMiniMacrosInterval->setEnabled(true);
            bMiniMacros->setStyleSheet("font-weight: bold;");
        }
    }
};

#endif // MINIMACROS
