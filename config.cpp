#include "config.h"

#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>

void Config::checkDir(QString qtPath, SdkInfo &info)
{
    //    qDebug() << qtPath;
        info.isvalid = false;
        QString dirName = GetFileNameFromPath(qtPath);
        if(dirName.startsWith("Qt")){

            // 解析版本
            QDir rootDir(qtPath);
            QString version = dirName;
            QStringList directories = rootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            for(auto p : directories){
                if(p.contains(".") && (p.split(".").size() == 3) ){
                    version = p;
                    break;
                }
            }

            QStringList versionNumbers = version.split(".");
            if(versionNumbers.size() == 3){
                info.path = qtPath;
                info.version = dirName;
                qDebug() << "解析到版本" << dirName;

                // 接着解析编译器
                QString complierPath = qtPath + "/" + version;
                QDir directory(complierPath);
                QStringList directories = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
                directories.removeOne("Src");

                // 遍历编译器目录，确认windeployqt路径是否有效
                for(auto p : directories){
                    QString windeployPath = QString("%1/%2/bin/windeployqt.exe").arg(complierPath).arg(p);
                    QFileInfo fileInfo(windeployPath);
                    if(fileInfo.exists() && fileInfo.isFile()){
                        Complier complier;
                        complier.name = p;
                        complier.path = complierPath + "/" + p;
                        complier.deployPath = windeployPath;

                        info.compliers << complier;
                    }
                }

                if(!info.compliers.isEmpty()){
                    info.isvalid = true;
                    info.defaltComplier = info.compliers[0];
                }
            }
        }
}

Config::Config(QObject *parent) : QObject(parent)
{
    read();
}

void Config::setQtPath(const QString path)
{
    QtConfigPath = path;

    emit changed();

    save();
}

void Config::setQtComplierPath(const QString name)
{
    QtComplierName = name;
    for(auto p : m_info.compliers){
        if(p.path == QtComplierName){
            m_info.defaltComplier = p;
            break;
        }
    }


//    emit changed();

    save();
}

void Config::setSdkInfo(const SdkInfo &info)
{
    m_info = info;

    save();

    emit changed();
}

void Config::save()
{

    QString fileName = QApplication::applicationDirPath() + "/config.json";
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file:" << file.errorString();
        return;
    }

    QJsonObject configObject;
    configObject["QtPath"] = m_info.path;
    configObject["Complier"] = m_info.defaltComplier.name;

    QJsonDocument doc(configObject);
    file.write(doc.toJson());
    file.close();
    qDebug() << "Config file written successfully.";
}

void Config::read()
{
    QString fileName = QApplication::applicationDirPath() + "/config.json";
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << file.errorString();
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject configObject = doc.object();

    QtConfigPath = configObject["QtPath"].toString();
    QtComplierName = configObject["Complier"].toString();

    checkDir(QtConfigPath, m_info);
    for(auto p : m_info.compliers){
        if(p.name == QtComplierName){
            m_info.defaltComplier = p;
            break;
        }
    }

    file.close();
}

QString Config::getQtConfigPath() const
{
    return QtConfigPath;
}

QString Config::getQtComplierName() const
{
    return QtComplierName;
}

SdkInfo Config::getInfo() const
{
    return m_info;
}

