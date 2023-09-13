#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "config.h"

#include <QDir>
#include <QFileDialog>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Qt打包工具");
    setWindowIcon(QIcon(":/logo.png"));

    dlg = new ConfigDlg(this);

    connect(&Config::getInstance(), &Config::changed, this, &MainWindow::slotUpdateContent);
    slotUpdateContent();

    connect(ui->labProgram, &DropLabel::fileSelected, this, &MainWindow::selectPrograme);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotUpdateContent()
{
    SdkInfo info = Config::getInstance().getInfo();
    bool isInit =info.isvalid;


    ui->frameTooltip->setVisible(!isInit);
    ui->frameContent->setEnabled(isInit);
    if(!isInit){
        return;
    }

    resize(0, 0);
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    resize(0, 0);


    // 更新当前Qt版本
    ui->editVersion->setText(info.version);

    // 更新编译器列表
    ui->comboComplier->clear();
    for(auto p : info.compliers){
        ui->comboComplier->addItem(p.name, p.deployPath);
    }

    QString complier = Config::getInstance().getInfo().defaltComplier.name;
    ui->comboComplier->setCurrentText(complier);

}

void MainWindow::selectPrograme(const QString fileName)
{
    programPath = fileName;
    QFileInfo info(fileName);

    QString name = info.fileName();
    QString path = info.absolutePath();

    if(name.isEmpty()){
        ui->btnDeploy->setEnabled(false);
    }

    ui->labProgram->setText(name);
    ui->labPath->setText(tr("路径：%1").arg(path));

    resize(0, 0);
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    resize(0, 0);
}


void MainWindow::on_actionConfig_triggered()
{
    dlg->exec();

    slotUpdateContent();

}

void MainWindow::on_btnChooseProgram_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "", "", tr("(*.exe)"));
    if(file.isEmpty()){
        return;
    }


    selectPrograme(file);
}

void MainWindow::on_btnDeploy_clicked()
{
    if(programPath.isEmpty()){
        return;
    }

    // 根据所选编译器得到windeployqt.exe路径
    QString windeployPath = QString("%1/bin/windeployqt.exe").arg(Config::getInstance().getQtComplierName());
    QFileInfo info(windeployPath);
    if(info.exists()){
        int code = QProcess::execute(windeployPath, QStringList() << programPath);
        if(code < 0){
            QMessageBox::information(this, "提示", "打包失败！");
        }
        else{
            QMessageBox::information(this, "提示", "打包完成！");
        }
    }
}

void MainWindow::on_btnTest_clicked()
{
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    int code = QProcess::execute(programPath);
    if(code < 0){
        QMessageBox::information(this, "提示", "测试失败");
    }
}

void MainWindow::on_btnConfig_clicked()
{
    on_actionConfig_triggered();
}

void MainWindow::on_action_triggered()
{
    // 自动检测SDK环境
    bool ret = false;

    // 检测C盘是否有Qt目录
    const QString defaultDir = "C:/Qt";
    QDir dir(defaultDir);
    if(dir.exists()){
         //检测Qt5.x.x
        QFileInfoList directories = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for(auto p : directories){
            SdkInfo info;
            Config::checkDir(p.absoluteFilePath(), info);
            if(info.isvalid){
                // 提示是否使用

                QStringList compliersList;
                for(auto p: info.compliers){
                    compliersList << p.name;
                }
                QString compliersName = compliersList.join(" ");
                QString content = QString("检测到%1，编译器 %2\n是否使用？").arg(info.version, compliersName);
                QMessageBox::StandardButton btn = QMessageBox::question(this, "提示", content);
                if(btn == QMessageBox::Yes){
                    Config::getInstance().setSdkInfo(info);
                }


            }
        }
    }
}
