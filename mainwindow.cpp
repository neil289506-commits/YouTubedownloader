#include "mainwindow.h"
#include <QTime>
#include <QStatusBar>
#include <QStandardPaths>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QSettings>
#include <QHeaderView>
#include <QDateTime>
#include <QMessageBox>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), downloadThread(nullptr), worker(nullptr) {
    
    setWindowTitle("YouTube 下載器 Pro");
    setWindowIcon(QIcon(":/icon.ico"));
    setMinimumSize(700, 750);
    
    setupUI();
    loadSettings();
    
    // 創建下載線程
    downloadThread = new QThread(this);
    worker = new DownloadWorker();
    worker->moveToThread(downloadThread);
    
    connect(downloadThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &MainWindow::destroyed, downloadThread, &QThread::quit);
    connect(worker, &DownloadWorker::finished, this, &MainWindow::onDownloadFinished);
    connect(worker, &DownloadWorker::progressUpdate, this, &MainWindow::onProgressUpdate);
    connect(worker, &DownloadWorker::logMessage, this, &MainWindow::onLogMessage);
    
    downloadThread->start();
}

MainWindow::~MainWindow() {
    saveSettings();
    if (downloadThread) {
        downloadThread->quit();
        downloadThread->wait();
    }
}

void MainWindow::setupUI() {
    QWidget *central = new QWidget;
    QVBoxLayout *main = new QVBoxLayout(central);
    main->setSpacing(10);
    main->setContentsMargins(15, 15, 15, 15);
    
    // URL 輸入區域
    QHBoxLayout *urlLayout = new QHBoxLayout;
    urlInput = new QLineEdit;
    urlInput->setPlaceholderText("輸入 YouTube 視頻或播放清單連結...");
    urlInput->setMinimumHeight(35);
    addBtn = new QPushButton("➕ 新增");
    addBtn->setMinimumWidth(100);
    addBtn->setMinimumHeight(35);
    removeBtn = new QPushButton("🗑️ 移除");
    removeBtn->setMinimumWidth(100);
    removeBtn->setMinimumHeight(35);
    
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddUrl);
    connect(removeBtn, &QPushButton::clicked, this, &MainWindow::onRemoveSelected);
    
    urlLayout->addWidget(new QLabel("連結:"), 0);
    urlLayout->addWidget(urlInput, 1);
    urlLayout->addWidget(addBtn);
    urlLayout->addWidget(removeBtn);
    main->addLayout(urlLayout);
    
    // URL 列表
    urlTable = new QTableWidget;
    urlTable->setColumnCount(2);
    urlTable->setHorizontalHeaderLabels(QStringList() << "✓" << "YouTube 連結");
    urlTable->horizontalHeader()->setStretchLastSection(true);
    urlTable->setColumnWidth(0, 50);
    urlTable->setMaximumHeight(150);
    urlTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    main->addWidget(urlTable);
    
    // 下載配置區
    QGroupBox *configGroup = new QGroupBox("下載設定");
    QVBoxLayout *configLayout = new QVBoxLayout;
    
    // 路徑選擇
    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathInput = new QLineEdit;
    pathInput->setMinimumHeight(30);
    QPushButton *browseBtn = new QPushButton("📁 瀏覽");
    browseBtn->setMaximumWidth(100);
    connect(browseBtn, &QPushButton::clicked, this, &MainWindow::onBrowseFolder);
    pathLayout->addWidget(new QLabel("下載位置:"), 0);
    pathLayout->addWidget(pathInput, 1);
    pathLayout->addWidget(browseBtn);
    configLayout->addLayout(pathLayout);
    
    // 格式和分辨率
    QHBoxLayout *formatLayout = new QHBoxLayout;
    
    formatCombo = new QComboBox;
    formatCombo->addItems(QStringList() << "mp4" << "mkv" << "webm" << "mp3" << "mpa");
    formatLayout->addWidget(new QLabel("格式:"), 0);
    formatLayout->addWidget(formatCombo, 0);
    
    resolutionCombo = new QComboBox;
    resolutionCombo->addItems(QStringList() << "2160p" << "1440p" << "1080p" << "720p" << "480p" << "360p");
    resolutionCombo->setCurrentText("1080p");
    formatLayout->addWidget(new QLabel("解析度:"), 0);
    formatLayout->addWidget(resolutionCombo, 0);
    
    fpsCombo = new QComboBox;
    fpsCombo->addItems(QStringList() << "60" << "50" << "40" << "30" << "20");
    formatLayout->addWidget(new QLabel("幀率:"), 0);
    formatLayout->addWidget(fpsCombo, 0);
    
    formatLayout->addStretch();
    configLayout->addLayout(formatLayout);
    
    // 選項
    QHBoxLayout *optionsLayout = new QHBoxLayout;
    playlistCheck = new QCheckBox("播放清單模式");
    playlistCheck->setChecked(true);
    autoSubtitlesCheck = new QCheckBox("下載字幕");
    autoTranslateCheck = new QCheckBox("自動翻譯");
    
    optionsLayout->addWidget(playlistCheck);
    optionsLayout->addWidget(autoSubtitlesCheck);
    optionsLayout->addWidget(autoTranslateCheck);
    optionsLayout->addStretch();
    configLayout->addLayout(optionsLayout);
    
    configGroup->setLayout(configLayout);
    main->addWidget(configGroup);
    
    // 進度和狀態
    progressBar = new QProgressBar;
    progressBar->setMaximumHeight(25);
    main->addWidget(progressBar);
    
    downloadInfoLabel = new QLabel("準備就緒");
    main->addWidget(downloadInfoLabel);
    
    // 控制按鈕
    QHBoxLayout *ctrlLayout = new QHBoxLayout;
    downloadBtn = new QPushButton("▶️ 開始下載");
    downloadBtn->setMinimumHeight(40);
    downloadBtn->setStyleSheet(
        "QPushButton { background-color: #00a055; font-weight: bold; }"
        "QPushButton:hover { background-color: #00c86b; }"
        "QPushButton:pressed { background-color: #008844; }");
    
    pauseBtn = new QPushButton("⏸ 暫停");
    pauseBtn->setMinimumHeight(40);
    pauseBtn->setEnabled(false);
    
    cancelBtn = new QPushButton("⏹ 取消");
    cancelBtn->setMinimumHeight(40);
    cancelBtn->setEnabled(false);
    
    settingsBtn = new QPushButton("⚙️ 設定");
    settingsBtn->setMaximumWidth(100);
    settingsBtn->setMinimumHeight(40);
    
    connect(downloadBtn, &QPushButton::clicked, this, &MainWindow::onStartDownload);
    connect(pauseBtn, &QPushButton::clicked, this, &MainWindow::onPauseDownload);
    connect(cancelBtn, &QPushButton::clicked, this, &MainWindow::onCancelDownload);
    connect(settingsBtn, &QPushButton::clicked, this, &MainWindow::onOpenSettings);
    
    ctrlLayout->addWidget(downloadBtn);
    ctrlLayout->addWidget(pauseBtn);
    ctrlLayout->addWidget(cancelBtn);
    ctrlLayout->addStretch();
    ctrlLayout->addWidget(settingsBtn);
    main->addLayout(ctrlLayout);
    
    // 日誌區域
    QGroupBox *logGroup = new QGroupBox("下載日誌");
    QVBoxLayout *logLayout = new QVBoxLayout;
    logBox = new QTextEdit;
    logBox->setReadOnly(true);
    logBox->setMinimumHeight(200);
    logBox->setStyleSheet("QTextEdit { font-family: Courier New; font-size: 9pt; }");
    logLayout->addWidget(logBox);
    
    QHBoxLayout *logBtnLayout = new QHBoxLayout;
    QPushButton *clearBtn = new QPushButton("清除日誌");
    connect(clearBtn, &QPushButton::clicked, logBox, &QTextEdit::clear);
    QPushButton *exportBtn = new QPushButton("匯出日誌");
    logBtnLayout->addStretch();
    logBtnLayout->addWidget(clearBtn);
    logBtnLayout->addWidget(exportBtn);
    logLayout->addLayout(logBtnLayout);
    
    logGroup->setLayout(logLayout);
    main->addWidget(logGroup);
    
    // 狀態欄
    statusLabel = new QLabel("就緒");
    statusBar()->addWidget(statusLabel);
    
    setCentralWidget(central);
}

void MainWindow::onAddUrl() {
    QString url = urlInput->text().trimmed();
    if (url.isEmpty()) {
        QMessageBox::warning(this, "警告", "請輸入有效的 YouTube 連結");
        return;
    }
    
    // 檢查是否已存在
    for (int i = 0; i < urlTable->rowCount(); ++i) {
        if (urlTable->item(i, 1)->text() == url) {
            QMessageBox::information(this, "提示", "此連結已在列表中");
            return;
        }
    }
    
    int row = urlTable->rowCount();
    urlTable->insertRow(row);
    
    QTableWidgetItem *checkItem = new QTableWidgetItem;
    checkItem->setCheckState(Qt::Checked);
    urlTable->setItem(row, 0, checkItem);
    
    QTableWidgetItem *urlItem = new QTableWidgetItem(url);
    urlItem->setFlags(urlItem->flags() & ~Qt::ItemIsEditable);
    urlTable->setItem(row, 1, urlItem);
    
    urlInput->clear();
    addLogEntry(QString("✓ 已新增: %1").arg(url));
}

void MainWindow::onRemoveSelected() {
    QList<int> rows;
    for (int i = 0; i < urlTable->rowCount(); ++i) {
        if (urlTable->item(i, 0)->checkState() == Qt::Checked) {
            rows.prepend(i);
        }
    }
    
    for (int row : rows) {
        urlTable->removeRow(row);
    }
    
    if (!rows.isEmpty()) {
        addLogEntry(QString("已移除 %1 項").arg(rows.count()));
    }
}

void MainWindow::onStartDownload() {
    if (urlTable->rowCount() == 0) {
        QMessageBox::warning(this, "警告", "請至少新增一個連結");
        return;
    }
    
    QStringList selectedUrls;
    for (int i = 0; i < urlTable->rowCount(); ++i) {
        if (urlTable->item(i, 0)->checkState() == Qt::Checked) {
            selectedUrls << urlTable->item(i, 1)->text();
        }
    }
    
    if (selectedUrls.isEmpty()) {
        QMessageBox::warning(this, "警告", "請選擇至少一個要下載的連結");
        return;
    }
    
    DownloadConfig config;
    config.path = pathInput->text();
    config.format = formatCombo->currentText();
    config.resolution = resolutionCombo->currentText();
    config.fps = fpsCombo->currentText().toInt();
    config.playlist = playlistCheck->isChecked();
    config.autoSubtitles = autoSubtitlesCheck->isChecked();
    config.autoTranslate = autoTranslateCheck->isChecked();
    config.urls = selectedUrls;
    
    isDownloading = true;
    downloadBtn->setEnabled(false);
    pauseBtn->setEnabled(true);
    cancelBtn->setEnabled(true);
    progressBar->setValue(0);
    
    addLogEntry(QString("開始下載 %1 個項目 | 格式: %2 | 分辨率: %3")
        .arg(selectedUrls.count()).arg(config.format).arg(config.resolution));
    
    QMetaObject::invokeMethod(worker, "startDownload", Qt::QueuedConnection,
                            Q_ARG(DownloadConfig, config));
}

void MainWindow::onPauseDownload() {
    if (isDownloading) {
        QMetaObject::invokeMethod(worker, "pauseDownload", Qt::QueuedConnection);
        pauseBtn->setEnabled(false);
    }
}

void MainWindow::onCancelDownload() {
    if (isDownloading) {
        QMetaObject::invokeMethod(worker, "cancelDownload", Qt::QueuedConnection);
        cancelBtn->setEnabled(false);
    }
}

void MainWindow::onDownloadFinished(bool success, const QString &msg) {
    isDownloading = false;
    downloadBtn->setEnabled(true);
    pauseBtn->setEnabled(false);
    cancelBtn->setEnabled(false);
    
    if (success) {
        addLogEntry("✓ " + msg);
        statusLabel->setText("下載完成");
        QMessageBox::information(this, "成功", msg);
    } else {
        addLogEntry("❌ " + msg);
        statusLabel->setText("下載失敗");
        QMessageBox::critical(this, "錯誤", msg);
    }
}

void MainWindow::onProgressUpdate(int current, int total, const QString &currentFile) {
    progressBar->setMaximum(total);
    progressBar->setValue(current);
    downloadInfoLabel->setText(
        QString("進度: %1/%2 | 當前: %3")
            .arg(current).arg(total).arg(currentFile.left(50) + "..."));
    statusLabel->setText(QString("下載中... (%1/%2)").arg(current).arg(total));
}

void MainWindow::onLogMessage(const QString &msg) {
    addLogEntry(msg);
}

void MainWindow::onBrowseFolder() {
    QString dir = QFileDialog::getExistingDirectory(this, "選擇下載位置");
    if (!dir.isEmpty()) {
        pathInput->setText(dir);
        addLogEntry(QString("下載位置已設定: %1").arg(dir));
    }
}

void MainWindow::onOpenSettings() {
    QMessageBox::information(this, "設定",
        "進階設定功能\n\n"
        "• 代理伺服器\n"
        "• Cookie 設定\n"
        "• 下載速度限制\n"
        "• 檔案命名規則\n\n"
        "此功能將在下一版本實現");
}

void MainWindow::addLogEntry(const QString &msg) {
    // QTime 才有唯一的 currentTime()
    QString timestamp = QTime::currentTime().toString("hh:mm:ss");
    logBox->append(QString("[%1] %2").arg(timestamp, msg));
}

void MainWindow::loadSettings() {
    QSettings settings("YTDownloader", "YTDownloader");
    pathInput->setText(settings.value("downloadPath", 
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/YTDownloads").toString());
    formatCombo->setCurrentText(settings.value("format", "mp4").toString());
    resolutionCombo->setCurrentText(settings.value("resolution", "1080p").toString());
    fpsCombo->setCurrentText(settings.value("fps", "60").toString());
    playlistCheck->setChecked(settings.value("playlist", true).toBool());
    autoSubtitlesCheck->setChecked(settings.value("subtitles", false).toBool());
    
    addLogEntry("✓ 設定已加載");
}

void MainWindow::saveSettings() {
    QSettings settings("YTDownloader", "YTDownloader");
    settings.setValue("downloadPath", pathInput->text());
    settings.setValue("format", formatCombo->currentText());
    settings.setValue("resolution", resolutionCombo->currentText());
    settings.setValue("fps", fpsCombo->currentText());
    settings.setValue("playlist", playlistCheck->isChecked());
    settings.setValue("subtitles", autoSubtitlesCheck->isChecked());
}
