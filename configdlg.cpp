#include "configdlg.h"
#include "config.h"
#include "mainwindow.h"
#include "ui_configdlg.h"

QString ConfigDlg::QtConfigPath   = "";
QString ConfigDlg::QtComplierName = "";

#include <QFileDialog>
#include <QMessageBox>

SdkInfo gInfo;
ConfigDlg::ConfigDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConfigDlg)
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
    ui->comboComplier->clear();
    auto info = Config::getInstance().getInfo();
    if ( !info.isvalid )
    {
        //        ui->comboComplier->addItem("未解析到编译器");
        return;
    }
    gInfo = info;
    ui->editQtPath->setText(info.path);

    for ( auto complier : info.compliers )
    {
        ui->comboComplier->addItem(complier.name, complier.path);
    }
    ui->comboComplier->setCurrentText(info.defaltComplier.name);
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
    if ( path.isEmpty() )
    {
        return;
    }

    ui->editQtPath->setText(path);
    on_btnParsePath_clicked();
}

void ConfigDlg::on_btnSave_clicked()
{
    QString complier = ui->comboComplier->currentData().toString();
    if ( complier.isEmpty() )
    {
        QMessageBox::information(this, "提示", "配置无效，请重新配置！");
    }

    Config::getInstance().setSdkInfo(gInfo);
    Config::getInstance().setQtComplierPath(complier);

    accept();
}

void ConfigDlg::on_btnCancel_clicked()
{
    close();
}

void ConfigDlg::on_btnParsePath_clicked()
{
    ui->comboComplier->clear();

    SdkInfo info;
    Config::checkDir(ui->editQtPath->text(), info);
    if ( info.isvalid )
    {
        gInfo = info;

        for ( auto complier : info.compliers )
        {
            ui->comboComplier->addItem(complier.name, complier.path);
        }

        ui->comboComplier->setCurrentText(info.defaltComplier.name);
    }
    else
    {
        //        ui->comboComplier->addItem("未解析到编译器");
        QMessageBox::information(this, "提示", "解析失败");
    }
}
