#include "downloadworker.h"
#include <QThread>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QUrl>
#include <QDebug>
#include <QRegularExpression>
#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

DownloadWorker::DownloadWorker(QObject *parent) 
    : QObject(parent), downloadProcess(nullptr), isPaused(false), isCancelled(false) {}

DownloadWorker::~DownloadWorker() {
    if (downloadProcess) {
        downloadProcess->kill();
        downloadProcess->waitForFinished();
        delete downloadProcess;
    }
}

bool DownloadWorker::setupDependencies() {
    emit logMessage("=================================================");
    emit logMessage("🔍 正在初始化系統環境...\n");

    // 💡 高速優化與動態原地更新版下載 Lambda
    auto downloadComponent = [this](const QString &stepName, const QString &url, const QString &outFile) -> bool {
        QProcess ps;
        
        // 內嵌高性能 PowerShell 腳本：大緩衝區(10MB)下載，並規範化輸出格式
        QString psScript = QString(
            "$url = '%1'; $file = '%2'; "
            "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; "
            "$wc = New-Object System.Net.WebClient; "
            "$wc.Headers.Add('User-Agent', 'Mozilla/5.0'); "
            "$sw = [System.Diagnostics.Stopwatch]::StartNew(); "
            "$received = 0; "
            "try { "
            "    $stream = $wc.OpenRead($url); "
            "    $total = [int64]$wc.ResponseHeaders['Content-Length']; "
            "    $fs = New-Object System.IO.FileStream($file, [System.IO.FileMode]::Create); "
            "    $buffer = New-Object byte[] 10485760; "
            "    while (($count = $stream.Read($buffer, 0, $buffer.Length)) -gt 0) { "
            "        $fs.Write($buffer, 0, $count); "
            "        $received += $count; "
            "        if ($sw.ElapsedMilliseconds -ge 150) { "
            "            $speed = ($received / 1024 / 1024) / ($sw.Elapsed.TotalSeconds); "
            "            $curMB = [Math]::Round($received / 1024 / 1024, 2); "
            "            $totMB = [Math]::Round($total / 1024 / 1024, 2); "
            "            $spdMB = [Math]::Round($speed, 2); "
            "            Write-Output \"PROGRESS:$spdMB MB/s $curMB MB/$totMB MB\"; "
            "        } "
            "    } "
            "    $fs.Close(); $stream.Close(); "
            "} catch { exit 1; }"
        ).arg(url, outFile);

        // 💡 修正關鍵點：加上 &ps 以便在 Lambda 內部讀取標準輸出
        connect(&ps, &QProcess::readyReadStandardOutput, [this, stepName, &ps]() {
            QString out = QString::fromLocal8Bit(ps.readAllStandardOutput());
            QStringList lines = out.split("\n", Qt::SkipEmptyParts);
            for (const QString &line : lines) {
                if (line.trimmed().startsWith("PROGRESS:")) {
                    QString data = line.trimmed().mid(9);
                    // 透過 UPDATE_PANEL 原地更新元件下載數據
                    emit logMessage("UPDATE_PANEL:\n" + stepName + "\nrequesting : " + data);
                }
            }
        });

#if defined(Q_OS_WIN)
        ps.setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args) {
            args->flags |= CREATE_NO_WINDOW;
        });
#endif

        emit logMessage("RESET_PANEL"); // 開啟元件動態面板
        ps.start("powershell", QStringList() << "-Command" << psScript);
        ps.waitForFinished(-1);
        emit logMessage("END_PANEL");   // 關閉元件動態面板

        return (ps.exitCode() == 0 && QFile::exists(outFile));
    };

    // ------------------ 1/2 yt-dlp 下載 ------------------
    if (!QFile::exists("yt-dlp.exe")) {
        if (!downloadComponent("1/2 yt-dlp download", "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe", "yt-dlp.exe")) {
            emit logMessage("❌ 1/2 yt-dlp 下載失敗");
            return false;
        }
        emit logMessage("✓ 1/2 yt-dlp 下載完成！\n");
    } else {
        emit logMessage("✓ 1/2 yt-dlp 元件已存在，跳過下載。");
    }

    // ------------------ 2/2 FFmpeg 下載與解壓 ------------------
    if (!QFile::exists("ffmpeg.exe") || !QFile::exists("ffprobe.exe")) {
        if (!downloadComponent("2/2 ffmpeg downloading", "https://github.com/GyanD/codexffmpeg/releases/download/6.0/ffmpeg-6.0-essentials_build.zip", "ffmpeg_temp.zip")) {
            emit logMessage("❌ 2/2 ffmpeg 下載失敗");
            return false;
        }

        // 解壓縮階段同樣採用動態原地更新
        emit logMessage("RESET_PANEL");
        emit logMessage("UPDATE_PANEL:\nExtracting : ffmpeg_temp.zip [ 0% ]");
        QThread::msleep(200);
        emit logMessage("UPDATE_PANEL:\nExtracting : ffmpeg_temp.zip [ 45% ]");

        QProcess unzip;
        unzip.start("powershell", QStringList() << "-Command" 
            << "Expand-Archive -Path 'ffmpeg_temp.zip' -DestinationPath 'ffmpeg_temp_dir' -Force");
        unzip.waitForFinished(-1);

        emit logMessage("UPDATE_PANEL:\nExtracting : ffmpeg_temp.zip [ 85% ]");

        try {
            for (auto& p : fs::recursive_directory_iterator("ffmpeg_temp_dir")) {
                if (p.path().filename() == "ffmpeg.exe") 
                    fs::rename(p.path(), "ffmpeg.exe");
                else if (p.path().filename() == "ffprobe.exe")
                    fs::rename(p.path(), "ffprobe.exe");
            }
            fs::remove("ffmpeg_temp.zip");
            fs::remove_all("ffmpeg_temp_dir");
            emit logMessage("UPDATE_PANEL:\nExtracting : ffmpeg_temp.zip [ 100% ]");
            emit logMessage("END_PANEL");
            emit logMessage("✓ 2/2 ffmpeg 元件部署成功！\n");
        } catch (...) {
            emit logMessage("END_PANEL");
            emit logMessage("⚠️ 檔案 IO 衝突，解壓失敗。");
            return false;
        }
    } else {
        emit logMessage("✓ 2/2 ffmpeg 元件已存在，跳過下載。");
    }

    emit logMessage("=================================================\n");
    return true;
}

QString DownloadWorker::buildCommand(const QString &url, const DownloadConfig &config) {
    QString cmd = "yt-dlp.exe ";
    
    // 如果本地有高速分割下載器 aria2c.exe 則自動協同調用
    if (QFile::exists("aria2c.exe")) {
        cmd += "--external-downloader aria2c --external-downloader-args \"-x " 
            + QString::number(config.threadCount) + " -s " + QString::number(config.threadCount) + "\" ";
    } else {
        cmd += "-N " + QString::number(config.threadCount) + " ";  
    }
    
    if (config.playlist) {
        cmd += "--yes-playlist ";
    } else {
        cmd += "--no-playlist ";
    }
    
    if (config.autoSubtitles) {
        cmd += "--write-subs --write-auto-subs --sub-langs \"zh-Hant,zh-Hans,en\" ";
    }
    if (config.autoTranslate) {
        cmd += "--convert-subs srt ";
    }
    
    bool isAudio = (config.format == "mp3" || config.format == "mpa");
    
    if (isAudio) {
        cmd += "-x --audio-format " + config.format + " ";
        if (config.playlist) {
            cmd += "-o \"%(playlist_index)s - %(title)s.%(ext)s\" ";
        } else {
            cmd += "-o \"%(title)s.%(ext)s\" ";
        }
    } else {
        if (config.playlist) {
            cmd += "-f \"bv*[ext=mp4]+ba[ext=m4a]/b[ext=mp4] / bv*+ba/b\" ";
            cmd += "-o \"%(playlist_index)s - %(title)s.%(ext)s\" ";
        } else {
            QString resNum = config.resolution;
            if (resNum.endsWith("p")) {
                resNum.chop(1);
            }
            cmd += QString("-f \"bv*[height<=%1][fps<=%2][ext=mp4]+ba[ext=m4a]/bv*[height<=%1][fps<=%2]+ba/b\" ")
                .arg(resNum, QString::number(config.fps));
            cmd += "-o \"%(title)s.%(ext)s\" ";
        }
        cmd += "--merge-output-format " + config.format + " ";
    }
    
    cmd += QString("-P \"" + config.path + "\" ");
    cmd += "\"" + url + "\"";
    return cmd;
}

void DownloadWorker::startDownload(const DownloadConfig &config) {
    isPaused = false;
    isCancelled = false;
    
    if (!setupDependencies()) {
        emit finished(false, "環境初始化失敗");
        return;
    }
    
    int total = config.urls.size();
    bool isAudio = (config.format == "mp3" || config.format == "mpa");
    QString modeStr = isAudio ? "純音樂下載 (Audio Mode)" : "高清影片下載 (Video Mode)";
    
    // 告知 UI 準備進入任務動態更新區塊
    emit logMessage("RESET_PANEL"); 

    for (int i = 0; i < total; ++i) {
        if (isCancelled) break;
        
        QString cmdStr = buildCommand(config.urls[i], config);
        downloadProcess = new QProcess;
        
        static QString lastPercent = ""; 
        
        connect(downloadProcess, &QProcess::readyReadStandardOutput, [=]() {
            QString output = QString::fromUtf8(downloadProcess->readAllStandardOutput());
            
            // 抓取 yt-dlp 進度行
            static QRegularExpression progressRegex(R"(\[download\]\s+(\d+\.\d+)%\s+of\s+(\d+\.\d+\w+)\s+at\s+(\d+\.\d+\w+\/s))");
            QRegularExpressionMatch match = progressRegex.match(output);
            
            if (match.hasMatch()) {
                QString percent = match.captured(1);
                QString totalSize = match.captured(2);
                QString speed = match.captured(3);
                
                if (percent == lastPercent) return;
                lastPercent = percent;
                
                // 統一面板格式，多個影片時 i+1 會自動往上加 (例如：[ 2 / 5 ])
                QString statusPanel = QString(
                    "UPDATE_PANEL:"
                    "=========================================\n"
                    " 模式: %1\n"
                    " 進度: [ %2 / %3 ] (%4%)\n"
                    " 線程: %5\n"
                    " 總速度 : %6\n"
                    " 檔案大小: %7\n"
                    "========================================="
                ).arg(modeStr).arg(i + 1).arg(total).arg(percent).arg(config.threadCount).arg(speed).arg(totalSize);
                 
                emit logMessage(statusPanel);
            }
        });

#if defined(Q_OS_WIN)
        downloadProcess->setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args) {
            args->flags |= CREATE_NO_WINDOW;
        });
#endif
        
        downloadProcess->startCommand(cmdStr);
        if (downloadProcess->waitForStarted()) {
            downloadProcess->waitForFinished(-1);
        }
        
        delete downloadProcess;
        downloadProcess = nullptr;
    }

    emit logMessage("END_PANEL"); // 解除動態覆寫控制

    if (isCancelled) {
        emit finished(false, "下載已取消");
        emit logMessage("❌ 下載已取消");
    } else {
        emit finished(true, "所有下載完成");
        emit logMessage("✓ 所有任務完成！");
    }
}

void DownloadWorker::pauseDownload() { isPaused = true; emit logMessage("⏸ 已暫停"); }
void DownloadWorker::cancelDownload() { isCancelled = true; if (downloadProcess) downloadProcess->kill(); emit logMessage("⏹ 正在取消任務..."); }