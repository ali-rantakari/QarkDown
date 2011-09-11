#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QDir>
#include <QStandardItemModel>

namespace Ui {
    class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QSettings *appSettings, QWidget *parent = 0);
    ~PreferencesDialog();

public slots:
    void accepted();
    void rejected();

private slots:
    void fontButtonClicked();
    void openStylesFolderButtonClicked();
    void stylesComboBoxCurrentIndexChanged(int index);

signals:
    void updated();

protected:
    void showEvent(QShowEvent *event);

private:
    void setupConnections();
    void updateUIFromSettings();
    void updateStylesComboBoxFromSettings();
    void updateStyleInfoTextFromComboBoxSelection();
    void updateSettingsFromUI();
    void setFontToLabel(QFont font);
    QSettings *settings;
    Ui::PreferencesDialog *ui;
    QDir userStylesDir();
    QStringList userStyleFiles();
    QStandardItemModel *stylesModel;
};

#endif // PREFERENCESDIALOG_H
