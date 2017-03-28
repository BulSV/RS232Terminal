#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QMouseEvent>

#include "DataTable.h"

#include <QDebug>

DataTable::DataTable(int columns, QWidget *parent)
    : QWidget(parent)
    , mouseSelectEnable(true)
{
    setColumns(columns);
}

int DataTable::getColumns() const
{
    return columns;
}

void DataTable::setLabels(const QList<QString> &labels)
{
    this->labelsText = labels;
    for(int i = 0; i < columns; ++i) {
        this->labels[i]->setText(labelsText.at(i));
    }
}

const QList<QString> &DataTable::getLabels() const
{
    return labelsText;
}

void DataTable::setData(const QList<QString> &data)
{
    this->data = data;
    QListIterator<QLineEdit*> itLE(cells);
    while(itLE.hasNext()) {
        itLE.next()->clear();
    }
    itLE.toFront();
    QListIterator<QString> itData(data);
    while(itLE.hasNext() && itData.hasNext()) {
        itLE.next()->setText(itData.next());
    }
}

const QList<QString> &DataTable::getData() const
{
    return data;
}

void DataTable::clearData()
{
    QListIterator<QLineEdit*> it(cells);
    while(it.hasNext()) {
        it.next()->clear();
    }
}

void DataTable::setValidator(const QValidator *validator)
{
    QListIterator<QLineEdit*> it(cells);
    while(it.hasNext()) {
        it.next()->setValidator(validator);
    }
}

void DataTable::setFont(const QFont &font)
{
    QListIterator<QLineEdit*> it(cells);
    while(it.hasNext()) {
        it.next()->setFont(font);
    }
}

const QFont &DataTable::getFont() const
{
    return cells.first()->font();
}

void DataTable::setDataAlignment(Qt::Alignment alignment)
{
    QListIterator<QLineEdit*> it(cells);
    while(it.hasNext()) {
        it.next()->setAlignment(alignment);
    }
}

void DataTable::setReadOnly(bool readOnly)
{
    QListIterator<QLineEdit*> it(cells);
    while(it.hasNext()) {
        it.next()->setReadOnly(readOnly);
    }
}

void DataTable::setSelected(bool selected)
{
    QListIterator<QLabel*> it(labels);
    QLabel *label = 0;
    while(it.hasNext()) {
        label = it.next();
        QFont font = label->font();
        font.setBold(selected);
        label->setFont(font);
    }

    emit tableSelected(selected);
}

void DataTable::setMouseSelectEnable(bool enable)
{
    mouseSelectEnable = enable;
}

void DataTable::setColumns(int columns)
{
    this->columns = columns;
    if(columns < 1) {
        this->columns = 1;
    }

    QHBoxLayout *labelsLayout = new QHBoxLayout;
    labelsLayout->setSpacing(0);
    labelsLayout->setContentsMargins(0, 0, 0, 0);
    QLabel *label = 0;

    QHBoxLayout *cellsLayout = new QHBoxLayout;
    cellsLayout->setSpacing(0);
    cellsLayout->setContentsMargins(0, 0, 0, 0);

    QLineEdit *cell = 0;
    for(int i = 0; i < columns; ++i) {
        label = new QLabel(this);
        labelsLayout->addWidget(label, 0, Qt::AlignCenter);
        labels.append(label);

        cell = new QLineEdit(this);
        cell->setAlignment(Qt::AlignCenter);
        cellsLayout->addWidget(cell);
        cells.append(cell);
        cell->installEventFilter(this);
        connect(cell, &QLineEdit::textChanged, this, &DataTable::sendDataChanged);
        connect(cell, &QLineEdit::editingFinished, this, &DataTable::editingFinished);
    }

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addItem(labelsLayout);
    mainLayout->addItem(cellsLayout);

    setLayout(mainLayout);
}

void DataTable::sendDataChanged()
{
    data.clear();
    QListIterator<QLineEdit*> it(cells);
    while(it.hasNext()) {
        data.append(it.next()->text());
    }

    emit dataChanged(data);
}

bool DataTable::eventFilter(QObject *obj, QEvent *e)
{
    QLineEdit *cell = qobject_cast<QLineEdit*>(obj);
    if(cell != 0) {
        int focusedCellIndex = cells.indexOf(cell);
        switch (static_cast<int>(e->type())) {
        case QEvent::FocusIn:
            setSelected(true);
            break;
        case QEvent::FocusOut:
            setSelected(false);
            break;
        case QEvent::KeyRelease:
        {
            if(!mouseSelectEnable) {
                break;
            }
            switch(static_cast<QKeyEvent*>(e)->key()) {
            case Qt::Key_Left:
                emit editingFinished();
                if(focusedCellIndex > 0) {
                    cells[focusedCellIndex - 1]->setFocus();
                    cells[focusedCellIndex - 1]->selectAll();
                } else {
                    cell->selectAll();
                }
                break;
            case Qt::Key_Right:
                emit editingFinished();
                if(focusedCellIndex < cells.size() - 1) {
                    cells[focusedCellIndex + 1]->setFocus();
                    cells[focusedCellIndex + 1]->selectAll();
                } else {
                    cell->selectAll();
                }
                break;
            }
            break;
        }
        case QEvent::MouseButtonRelease:
            if(static_cast<QMouseEvent*>(e)->button() == Qt::LeftButton && mouseSelectEnable) {
                emit editingFinished();
                cell->selectAll();
            }
            break;
        }
    }

    return QObject::eventFilter(obj, e);
}
