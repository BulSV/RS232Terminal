#ifndef MACROS_EDIT_WIDGET_H
#define MACROS_EDIT_WIDGET_H

#include <QMainWindow>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>
#include <QByteArray>
#include <QCheckBox>
#include <QFileDialog>
#include <QSettings>

#include "DataTable.h"
#include "AsciiEncoder.h"
#include "HexEncoder.h"
#include "DecEncoder.h"
#include "MacrosRawEditWidget.h"
#include "OpenMacros.h"
#include "SaveMacros.h"

class MacrosEditWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit MacrosEditWidget(QWidget *parent = 0);
    const QByteArray &getPackage();
    void saveSettings(QSettings *settings, int macrosIndex);
    void loadSettings(QSettings *settings, int macrosIndex);
    void openMacrosFile(const QString &fileName);
signals:
    void titleChanged(const QString &title);
private:
    QToolBar *toolBar;
    QAction *actionNew;
    QAction *actionLoad;
    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionRawEdit;
    QAction *actionClearSelectedGroup;
    QAction *actionCR;
    QAction *actionLF;
    QAction *actionAddGroup;
    QAction *actionDeleteGroup;
    DataTable *tableAscii;
    DataTable *tableHex;
    DataTable *tableDec;
    QList<DataTable*> macrosGroups;
    QVBoxLayout *macrosLayout;
    DataTable *currentEditGroup;
    AsciiEncoder *asciiEncoder;
    HexEncoder *hexEncoder;
    DecEncoder *decEncoder;
    QByteArray package;
    bool CR;
    bool LF;
    MacrosRawEditWidget *macrosRawEditWidget;
    OpenMacros openMacros;
    SaveMacros saveMacros;
    QString macrosesOpenDir;
    QString macrosesSaveDir;
    QFileDialog *fileOpenDialog;
    QFileDialog *fileSaveAsDialog;
    QString macrosFileName;

    void connections();
    void view();

    void selectGroup();
    void clearSelectedGroup();
    void addGroup();
    void deleteGroup();
    QList<QString> getLabels() const;
    QList<QString> toUpper(const QList<QString> &source) const;
    void fromAsciiInput();
    void fromHexInput();
    void fromDecInput();
    void nonPrintableCharacters();
    QList<QString> getAsciiString();
    void fillEmptyBytes(DataTable *dataTable);
    void addCR(bool CR);
    void addLF(bool LF);
    QString fromStringListToString(DataTable *dataTable);
    void fillEditGroup();
    void setRawData(const QByteArray &rawData);
    void onEditRawData();
    void formPackage();
    void addCR_LF();
    void newMacrosFile();
    void openMacrosFile();
    void saveMacrosFile();
    void saveAsMacrosFile();
    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *e);
};

#endif // MACROS_EDIT_WIDGET_H
