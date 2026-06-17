#pragma once

#include <QObject>
#include <QString>
#include <QProcess>

struct DownloadConfig {
    QString path;
    QString format;
    QString resolution;
    int fps;
    bool playlist;
    bool autoSubtitles;
    bool autoTranslate;
    QStringList urls;
};

class DownloadWorker : public QObject {
    Q_OBJECT

public:
    explicit DownloadWorker(QObject *parent = nullptr);
    ~DownloadWorker();

public slots:
    void startDownload(const DownloadConfig &config);
    void pauseDownload();
    void cancelDownload();

signals:
    void finished(bool success, const QString &message);
    void progressUpdate(int current, int total, const QString &currentFile);
    void logMessage(const QString &msg);

private:
    bool setupDependencies();
    QString buildCommand(const QString &url, const DownloadConfig &config);
    
    QProcess *downloadProcess;
    bool isPaused = false;
    bool isCancelled = false;
};
