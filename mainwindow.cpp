#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "config.h"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

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

    process = new QProcess();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete process;
}

void MainWindow::slotUpdateContent()
{
    SdkInfo info   = Config::getInstance().getInfo();
    bool    isInit = info.isvalid;

    ui->frameTooltip->setVisible(!isInit);
    ui->frameContent->setEnabled(isInit);

    resize(0, 0);
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    resize(0, 0);
    int height = isInit ? 429 : 490;
    setFixedSize(width(), height);

    if ( !isInit )
    {
        return;
    }

    // 更新当前Qt版本
    ui->editVersion->setText("Qt" + info.version);
    ui->labCurVersion->setText(info.version);

    // 更新编译器列表
    ui->comboComplier->clear();
    for ( auto p : info.compliers )
    {
        ui->comboComplier->addItem(p.name, p.deployPath);
    }

    QString complier = Config::getInstance().getInfo().defaltComplier.name;
    ui->comboComplier->setCurrentText(complier);
    if ( complier.contains("mingw") )
    {
        ui->labCurComplier->setText("Mingw");
    }
    else if ( complier.contains("msvc") )
    {
        ui->labCurComplier->setText(complier.split("msvc").first());
    }
    else
    {
        ui->labCurComplier->setText(complier);
    }
}

void MainWindow::selectPrograme(const QString fileName)
{
    programPath = fileName;
    QFileInfo info(fileName);

    QString name = info.fileName();
    QString path = info.absolutePath();

    if ( name.isEmpty() )
    {
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
    if ( QDialog::Accepted == dlg->exec() )
    {
        slotUpdateContent();
    }
}

void MainWindow::on_btnChooseProgram_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "", "", tr("(*.exe)"));
    if ( file.isEmpty() )
    {
        return;
    }

    selectPrograme(file);
}

void MainWindow::on_btnDeploy_clicked()
{
    if ( programPath.isEmpty() )
    {
        QMessageBox::warning(this, "提示", "请选择程序！");
        return;
    }

    // 根据所选编译器得到windeployqt.exe路径
    QString   windeployPath = QString("%1/bin/windeployqt.exe").arg(Config::getInstance().getInfo().defaltComplier.path);
    QFileInfo info(windeployPath);
    if ( info.exists() )
    {

        process->setProgram(windeployPath);
        process->setArguments(QStringList() << programPath);
        connect(process, &QProcess::readyReadStandardOutput, this, [=]() {
            qDebug() << "读取输出";
            //            ui->textEdit->append(process->readAllStandardOutput());
        });
        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int code, QProcess::ExitStatus exitStatus) {
            qDebug() << code << exitStatus;
            qDebug() << "执行结束";
            if ( code >= 0 && QProcess::ExitStatus::NormalExit == exitStatus )
            {
                //                QMessageBox::information(this, "提示", "打包完成！");
            }
            else
            {
                //                QMessageBox::information(this, "提示", "打包失败！");
            }
        });

        process->start();   // 异步的方式调用

        //        int code = QProcess::execute(windeployPath, QStringList() << programPath);
        //        if ( code < 0 )
        //        {
        //            QMessageBox::information(this, "提示", "打包失败！");
        //        }
        //        else
        //        {
        //            QMessageBox::information(this, "提示", "打包完成！");
        //        }
    }
    else
    {
        QMessageBox::warning(this, "提示", "编译器路径错误，打包失败！");
    }
}

void MainWindow::on_btnTest_clicked()
{
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    int code = QProcess::execute(programPath);
    if ( code < 0 )
    {
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
    QMessageBox::StandardButton btn = QMessageBox::question(this, "提示", "自动检测只适用于C盘Qt默认安装的情况，是否继续？");
    if ( btn != QMessageBox::Yes )
    {
        return;
    }
    bool ret = false;

    // 检测C盘是否有Qt目录
    const QString defaultDir = "C:/Qt";
    QDir          dir(defaultDir);
    if ( dir.exists() )
    {
        //检测Qt5.x.x
        QFileInfoList directories = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for ( auto p : directories )
        {
            SdkInfo info;
            Config::checkDir(p.absoluteFilePath(), info);
            ret = info.isvalid;
            if ( info.isvalid )
            {
                // 提示是否使用

                QStringList compliersList;
                for ( auto p : info.compliers )
                {
                    compliersList << p.name;
                }
                QString                     compliersName = compliersList.join(" ");
                QString                     content       = QString("检测到%1，编译器 %2\n是否使用？").arg(info.version, compliersName);
                QMessageBox::StandardButton btn           = QMessageBox::question(this, "提示", content);
                if ( btn == QMessageBox::Yes )
                {
                    Config::getInstance().setSdkInfo(info);
                }
            }
        }
    }

    if ( !ret )
    {
        QMessageBox::warning(this, "提示", "自动检测失败！");
    }
}

void MainWindow::on_actionHelp_triggered()
{
}
