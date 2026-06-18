#pragma once

#include <QObject>
#include <QString>
#include <QProcess>
#include <QStringList>

struct DownloadConfig {
    QString path;
    QString format;
    QString resolution;
    int fps;
    int threadCount;        
    bool playlist;
    bool autoSubtitles;
    bool autoTranslate;
    QStringList urls;
};

class DownloadWorker : public QObject {
    Q_OBJECT

public:
    explicit DownloadWorker(QObject *parent = nullptr); // 💡 修正：標準 Qt 建構子
    ~DownloadWorker();

public slots:
    void startDownload(const DownloadConfig &config); // 💡 與實作檔、invokeMethod 參數完全對齊
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