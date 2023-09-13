#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "configdlg.h"
#include "config.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void slotUpdateContent();

    void selectPrograme(const QString fileName);



private:


private slots:
    void on_actionConfig_triggered();

    void on_btnChooseProgram_clicked();

    void on_btnDeploy_clicked();

    void on_btnTest_clicked();

    void on_btnConfig_clicked();

    void on_action_triggered();

private:
    Ui::MainWindow *ui;
    ConfigDlg *dlg;

    QString programPath;
};
#endif // MAINWINDOW_H
