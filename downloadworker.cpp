// 補上這個標頭檔
#include <QThread>
#include "downloadworker.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QUrl>
#include <QDebug>
#include <filesystem>
#include <windows.h>
#include <urlmon.h>

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ole32.lib")

namespace fs = std::filesystem;

DownloadWorker::DownloadWorker(QObject *parent) 
    : QObject(parent), downloadProcess(nullptr), isPaused(false), isCancelled(false) {
    setupDependencies();
}

DownloadWorker::~DownloadWorker() {
    if (downloadProcess) {
        downloadProcess->kill();
        downloadProcess->waitForFinished();
        delete downloadProcess;
    }
}

bool DownloadWorker::setupDependencies() {
    emit logMessage("初始化系統環境...");
    
    bool needsSetup = false;
    
    // 檢查 yt-dlp
    if (!QFile::exists("yt-dlp.exe")) {
        emit logMessage("正在下載 yt-dlp...");
        HRESULT hr = URLDownloadToFileW(nullptr, 
            L"https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe", 
            L"yt-dlp.exe", 0, nullptr);
        if (FAILED(hr)) {
            emit logMessage("❌ yt-dlp 下載失敗");
            return false;
        }
        emit logMessage("✓ yt-dlp 下載完成");
        needsSetup = true;
    }
    
    // 檢查 FFmpeg
    if (!QFile::exists("ffmpeg.exe") || !QFile::exists("ffprobe.exe")) {
        emit logMessage("正在下載 FFmpeg...");
        HRESULT hr = URLDownloadToFileW(nullptr,
            L"https://github.com/GyanD/codexffmpeg/releases/download/6.0/ffmpeg-6.0-essentials_build.zip",
            L"ffmpeg_temp.zip", 0, nullptr);
        if (SUCCEEDED(hr)) {
            QProcess proc;
            proc.start("powershell", QStringList() << "-Command" 
                << "Expand-Archive -Path 'ffmpeg_temp.zip' -DestinationPath 'ffmpeg_temp_dir' -Force");
            proc.waitForFinished();
            
            try {
                for (auto& p : fs::recursive_directory_iterator("ffmpeg_temp_dir")) {
                    if (p.path().filename() == "ffmpeg.exe") 
                        fs::rename(p.path(), "ffmpeg.exe");
                    else if (p.path().filename() == "ffprobe.exe")
                        fs::rename(p.path(), "ffprobe.exe");
                }
                fs::remove("ffmpeg_temp.zip");
                fs::remove_all("ffmpeg_temp_dir");
            } catch (...) {}
            emit logMessage("✓ FFmpeg 設定完成");
            needsSetup = true;
        }
    }
    
    return true;
}

QString DownloadWorker::buildCommand(const QString &url, const DownloadConfig &config) {
    QString cmd = "yt-dlp.exe ";
    
    cmd += config.playlist ? "--yes-playlist " : "--no-playlist ";
    
    if (config.autoSubtitles) cmd += "--write-subs --sub-langs all ";
    if (config.autoTranslate) cmd += "--convert-subs=srt ";
    
    if (config.format == "mp3" || config.format == "mpa") {
        cmd += QString("-x --audio-format %1 ").arg(config.format);
    } else {
        QString resNum = config.resolution.left(config.resolution.length() - 1);
        cmd += QString("-f \"bestvideo[height<=%1][fps<=%2]+bestaudio/best\" "
                      "--merge-output-format %3 ")
            .arg(resNum, QString::number(config.fps), config.format);
    }
    
    cmd += QString("--progress -o \"%1/%%(title)s.%%(ext)s\" %2")
        .arg(config.path, url);
    
    return cmd;
}

void DownloadWorker::startDownload(const DownloadConfig &config) {
    if (isCancelled) isCancelled = false;
    
    emit logMessage("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    emit logMessage("開始下載任務");
    
    int total = config.urls.length();
    for (int i = 0; i < total; ++i) {
        if (isCancelled) break;
        
        while (isPaused && !isCancelled) QThread::msleep(500);
        
        emit progressUpdate(i + 1, total, config.urls[i]);
        
        QString cmdStr = buildCommand(config.urls[i], config);
        emit logMessage(QString("【%1/%2】 %3").arg(i+1).arg(total).arg(config.urls[i]));
        
downloadProcess = new QProcess;
        connect(downloadProcess, &QProcess::finished, this,
                [this](int exitCode, QProcess::ExitStatus exitStatus) { 
                    if (downloadProcess) {
                        delete downloadProcess;
                        downloadProcess = nullptr;
                    }
                });
        
        downloadProcess->start(cmdStr);
        downloadProcess->waitForFinished(-1);
    }
    
    if (isCancelled) {
        emit finished(false, "下載已取消");
        emit logMessage("❌ 下載已取消");
    } else {
        emit finished(true, "所有下載完成");
        emit logMessage("✓ 所有任務完成");
    }
    emit logMessage("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
}

void DownloadWorker::pauseDownload() {
    isPaused = true;
    emit logMessage("⏸ 已暫停");
}

void DownloadWorker::cancelDownload() {
    isCancelled = true;
    if (downloadProcess) {
        downloadProcess->kill();
    }
    emit logMessage("⏹ 正在停止...");
}
