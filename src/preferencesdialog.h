#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QSettings>

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
    void fontButtonClicked();

signals:
    void updated();

private:
    void setupConnections();
    void updateUIFromSettings();
    void updateSettingsFromUI();
    void setFontToLabel(QFont font);
    QSettings *settings;
    Ui::PreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
