#pragma once

#include <QStatusBar>
#include <QStandardPaths>
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include <QThread>
#include <QProgressBar>
#include <memory>
#include "downloadworker.h"
#include "settingsdialog.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddUrl();
    void onStartDownload();
    void onPauseDownload();
    void onCancelDownload();
    void onDownloadFinished(bool success, const QString &msg);
    void onProgressUpdate(int current, int total, const QString &currentFile);
    void onLogMessage(const QString &msg);
    void onBrowseFolder();
    void onOpenSettings();
    void onRemoveSelected();
    void applySettingsChanges();

private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    void addLogEntry(const QString &msg);
    void applyFontSettings();
    
    // UI Components
    QLineEdit *urlInput;
    QLineEdit *pathInput;
    QTableWidget *urlTable;
    QComboBox *formatCombo, *resolutionCombo, *fpsCombo;
    QCheckBox *playlistCheck, *autoSubtitlesCheck, *autoTranslateCheck;
    QPushButton *addBtn, *removeBtn, *downloadBtn, *pauseBtn, *cancelBtn, *settingsBtn;
    QTextEdit *logBox;
    QProgressBar *progressBar;
    QLabel *statusLabel, *downloadInfoLabel;
    
    // Worker thread
    QThread *downloadThread;
    DownloadWorker *worker;
    
    // Settings
    SettingsDialog *settingsDialog;
    int threadCount = 4;
    QString fontFamily = "Microsoft JhengHei UI";
    int fontSize = 10;
    
    bool isDownloading = false;
};
