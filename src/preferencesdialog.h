#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QDir>
#include <QStandardItemModel>

#include "markdowncompiler.h"

namespace Ui {
    class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QSettings *appSettings, MarkdownCompiler *aCompiler, QWidget *parent = 0);
    ~PreferencesDialog();

public slots:
    void accepted();
    void rejected();

private slots:
    void fontButtonClicked();
    void openStylesFolderButtonClicked();
    void openCompilersFolderButtonClicked();
    void changeNotesFolderButtonClicked();
    void editHTMLTemplateButtonClicked();
    void stylesComboBoxCurrentIndexChanged(int index);
    void compilersComboBoxCurrentIndexChanged(int index);

signals:
    void updated();

protected:
    void showEvent(QShowEvent *event);

private:
    void setupConnections();
    void updateUIFromSettings();
    void updateStylesComboBoxFromSettings();
    void updateStyleInfoTextFromComboBoxSelection();
    void updateCompilersComboBoxFromSettings();
    void updateCompilerArgsFieldFromComboBoxSelection();
    void updateSettingsFromUI();
    void setFontToLabel(QFont font);
    void openFolderEnsuringItExists(QString path);
    void openPath(QString path, bool isFolder = false);
    QDir userStylesDir();
    QStringList userStyleFiles();
    QDir userCompilersDir();
    QStringList userCompilerFiles();

    Ui::PreferencesDialog *ui;
    QSettings *settings;
    MarkdownCompiler *compiler;
    QStandardItemModel *stylesComboBoxModel;
    QStandardItemModel *compilersComboBoxModel;
};

#endif // PREFERENCESDIALOG_H
