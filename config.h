#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

struct Complier
{
    QString name;
    QString path;
    QString deployPath;
};

struct SdkInfo
{
    QString version;
    QString path;
    QList<Complier> compliers;
    Complier defaltComplier;
    bool isvalid = false;
};

class Config : public QObject
{
    Q_OBJECT
public:
    static Config& getInstance()
    {
        static Config config;
        return config;
    }


    static void checkDir(QString qtPath, SdkInfo &info);

    static QString GetFileNameFromPath(const QString& path) {
        int lastSlashIndex = path.lastIndexOf('/');
        if (lastSlashIndex != -1) {
            QString fileName = path.mid(lastSlashIndex + 1);
            return fileName;
        }
        return path; // 没有斜杠，则直接返回整个路径作为文件名
    }

    explicit Config(QObject *parent = nullptr);

    void setQtPath(const QString path);
    void setQtComplierPath(const QString name);

    void setSdkInfo(const SdkInfo &info);

    void save();
    void read();

    QString getQtConfigPath() const;

    QString getQtComplierName() const;

    SdkInfo getInfo() const;

signals:
    void changed();

private:
    QString QtConfigPath;
    QString QtComplierName;
    SdkInfo m_info;

};

#endif // CONFIG_H
