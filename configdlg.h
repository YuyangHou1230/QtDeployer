#ifndef CONFIGDLG_H
#define CONFIGDLG_H

#include <QDialog>

namespace Ui {
    class ConfigDlg;
}

class ConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDlg(QWidget *parent = nullptr);
    ~ConfigDlg();

    static QString QtConfigPath;
    static QString QtComplierName;

private slots:
    void slotUpdateContent();

    void on_editQtPath_textChanged(const QString &arg1);

    void on_editComplierPath_textChanged(const QString &arg1);

    void on_btnSelectQt_clicked();

    void on_btnSelectComplier_clicked();

    void on_btnSave_clicked();

    void on_btnCancel_clicked();

private:
    Ui::ConfigDlg *ui;
};

#endif // CONFIGDLG_H
