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
    cbCheckInterval(int index, bool check);
    cbCheckPeriod(int index, bool check);

public:
    int index;
    QHBoxLayout *layout;
    QCheckBox *cbMiniMacrosInterval;
    QCheckBox *cbMiniMacrosPeriod;
    QPushButton *bMiniMacros;

    MiniMacros(int i, QString title, QWidget *parent = 0) : QWidget(parent)
    {
        index = i;
        cbMiniMacrosInterval = new QCheckBox;
        cbMiniMacrosPeriod = new QCheckBox;
        bMiniMacros = new QPushButton;
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
        connect(cbMiniMacrosInterval, SIGNAL(toggled(bool)), this, SLOT(ckeckInterval(bool)));
        connect(cbMiniMacrosPeriod, SIGNAL(toggled(bool)), this, SLOT(ckeckPeriod(bool)));
    }

public slots:
    void click() { emit bPress(index); }
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
