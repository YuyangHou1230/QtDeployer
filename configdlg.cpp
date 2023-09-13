#include "configdlg.h"
#include "ui_configdlg.h"
#include "config.h"
#include "mainwindow.h"

QString ConfigDlg::QtConfigPath = "";
QString ConfigDlg::QtComplierName = "";

#include <QFileDialog>
#include <QMessageBox>

ConfigDlg::ConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDlg)
{
    ui->setupUi(this);


    connect(&Config::getInstance(), &Config::changed, this, &ConfigDlg::slotUpdateContent);
    slotUpdateContent();
}

ConfigDlg::~ConfigDlg()
{
    delete ui;
}

void ConfigDlg::slotUpdateContent()
{
    auto info = Config::getInstance().getInfo();
    ui->editQtPath->setText(info.path);
    ui->editComplierPath->setText(info.defaltComplier.path);
}

void ConfigDlg::on_editQtPath_textChanged(const QString &arg1)
{
    QtConfigPath = arg1;

//    Config::getInstance().setQtPath(arg1);
}

void ConfigDlg::on_editComplierPath_textChanged(const QString &arg1)
{
    QtComplierName = arg1;

    Config::getInstance().setQtComplierPath(arg1);
}

void ConfigDlg::on_btnSelectQt_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "", "C:");
    if(path.isEmpty()){
        return;
    }

    ui->editQtPath->setText(path);
}

void ConfigDlg::on_btnSelectComplier_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "", QtConfigPath);

    ui->editComplierPath->setText(path);
}

void ConfigDlg::on_btnSave_clicked()
{
    QString complier = ui->editComplierPath->text();
    SdkInfo info;
    Config::checkDir(ui->editQtPath->text(), info);
    if(info.isvalid){
        Config::getInstance().setSdkInfo(info);
        Config::getInstance().setQtComplierPath(complier);

        accept();
    }
    else{
        QMessageBox::information(this, "提示", "解析失败");
    }
}

void ConfigDlg::on_btnCancel_clicked()
{
    close();
}
