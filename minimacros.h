#ifndef MINIMACROS
#define MINIMACROS

#include <QHBoxLayout>
#include <QCheckBox>
#include <QPushButton>

class MiniMacros : public QWidget
{
    Q_OBJECT
signals:
    bPress(int index);
    cbCheck(int index, bool check);

public:
    int index;
    QHBoxLayout *layout;
    QCheckBox *cbMiniMacros;
    QPushButton *bMiniMacros;

    MiniMacros(int i, QString title, QWidget *parent = 0) : QWidget(parent)
    {
        index = i;
        cbMiniMacros = new QCheckBox;
        bMiniMacros = new QPushButton;
        layout = new QHBoxLayout;
        bMiniMacros->setText(title);

        layout->setSpacing(2);
        layout->setMargin(2);

        layout->addWidget(cbMiniMacros);
        layout->addWidget(bMiniMacros);
        setLayout(layout);

        connect(bMiniMacros, SIGNAL(clicked(bool)), this, SLOT(click()));
        connect(cbMiniMacros, SIGNAL(toggled(bool)), this, SLOT(ckeck(bool)));
    }

public slots:
    void click() { emit bPress(index); }
    void ckeck(bool check) { emit cbCheck(index, check); }
};

#endif // MINIMACROS
