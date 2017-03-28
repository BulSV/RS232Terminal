#ifndef DATA_TABLE_H
#define DATA_TABLE_H

#include <QWidget>
#include <QList>
#include <QLabel>
#include <QLineEdit>
#include <QEvent>

class DataTable : public QWidget
{
    Q_OBJECT
public:
    explicit DataTable(int columns, QWidget *parent = 0);
    int getColumns() const;
    void setLabels(const QList<QString> &labels);
    const QList<QString> &getLabels() const;
    void setData(const QList<QString> &data);
    const QList<QString> &getData() const;
    void clearData();
    void setValidator(const QValidator *validator);
    void setFont(const QFont &font);
    const QFont &getFont() const;
    void setDataAlignment(Qt::Alignment alignment);
    void setReadOnly(bool readOnly);
    void setSelected(bool selected);
    void setMouseSelectEnable(bool enable);
signals:
    void dataChanged(const QList<QString> &data);
    void tableSelected(bool selected);
    void editingFinished();
private:
    int columns;
    bool mouseSelectEnable;
    QList<QString> labelsText;
    QList<QLineEdit*> cells;
    QList<QLabel*> labels;
    QList<QString> data;

    void setColumns(int columns);
    void sendDataChanged();

    // QObject interface
public:
    virtual bool eventFilter(QObject *obj, QEvent *e);
};

#endif // DATA_TABLE_H
