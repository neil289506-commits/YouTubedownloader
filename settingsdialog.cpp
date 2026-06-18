#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QSettings>
#include <QFont>
#include <QStandardPaths>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent), currentThreadCount(4), currentFontSize(10) {
    
    setWindowTitle("设定 - YouTube 下载器 Pro");
    setModal(true);
    setMinimumWidth(500);
    setMinimumHeight(400);
    
    setupUI();
    loadSettings();
}

SettingsDialog::~SettingsDialog() {}

void SettingsDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // ========== 线程设置 ==========
    QGroupBox *threadGroup = new QGroupBox("下载线程设置");
    QVBoxLayout *threadLayout = new QVBoxLayout(threadGroup);
    
    QHBoxLayout *sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(new QLabel("线程数:"));
    
    threadSlider = new QSlider(Qt::Horizontal);
    threadSlider->setMinimum(1);
    threadSlider->setMaximum(32);
    threadSlider->setValue(4);
    threadSlider->setTickPosition(QSlider::TicksBelow);
    threadSlider->setTickInterval(4);
    sliderLayout->addWidget(threadSlider);
    
    threadSpinBox = new QSpinBox;
    threadSpinBox->setMinimum(1);
    threadSpinBox->setMaximum(32);
    threadSpinBox->setValue(4);
    threadSpinBox->setFixedWidth(60);
    sliderLayout->addWidget(threadSpinBox);
    sliderLayout->addWidget(new QLabel("个"));
    
    threadLayout->addLayout(sliderLayout);
    
    threadInfoLabel = new QLabel("当前: 4 线程 (推荐: 4-8 个)");
    threadInfoLabel->setStyleSheet("color: #888; font-size: 11px;");
    threadLayout->addWidget(threadInfoLabel);
    
    threadGroup->setLayout(threadLayout);
    mainLayout->addWidget(threadGroup);
    
    // ========== 字体设置 ==========
    QGroupBox *fontGroup = new QGroupBox("字体设置");
    QVBoxLayout *fontLayout = new QVBoxLayout(fontGroup);
    
    QHBoxLayout *fontFamilyLayout = new QHBoxLayout;
    fontFamilyLayout->addWidget(new QLabel("字体:"));
    fontComboBox = new QFontComboBox;
    fontFamilyLayout->addWidget(fontComboBox);
    fontLayout->addLayout(fontFamilyLayout);
    
    QHBoxLayout *fontSizeLayout = new QHBoxLayout;
    fontSizeLayout->addWidget(new QLabel("大小:"));
    
    fontSizeSlider = new QSlider(Qt::Horizontal);
    fontSizeSlider->setMinimum(8);
    fontSizeSlider->setMaximum(24);
    fontSizeSlider->setValue(10);
    fontSizeSlider->setTickPosition(QSlider::TicksBelow);
    fontSizeSlider->setTickInterval(2);
    fontSizeLayout->addWidget(fontSizeSlider);
    
    fontSizeSpinBox = new QSpinBox;
    fontSizeSpinBox->setMinimum(8);
    fontSizeSpinBox->setMaximum(24);
    fontSizeSpinBox->setValue(10);
    fontSizeSpinBox->setSuffix(" pt");
    fontSizeSpinBox->setFixedWidth(70);
    fontSizeLayout->addWidget(fontSizeSpinBox);
    
    fontLayout->addLayout(fontSizeLayout);
    
    fontPreviewLabel = new QLabel("字体预览: YouTube 下载器 Pro");
    QFont previewFont;
    previewFont.setPointSize(10);
    fontPreviewLabel->setFont(previewFont);
    fontPreviewLabel->setStyleSheet("background-color: #f5f5f5; padding: 8px; border: 1px solid #ddd;");
    fontLayout->addWidget(fontPreviewLabel);
    
    fontGroup->setLayout(fontLayout);
    mainLayout->addWidget(fontGroup);
    
    // ========== 下载路径 ==========
    QGroupBox *pathGroup = new QGroupBox("下载路径");
    QVBoxLayout *pathLayout = new QVBoxLayout(pathGroup);
    
    QHBoxLayout *pathInputLayout = new QHBoxLayout;
    pathLabel = new QLabel("未设定");
    pathLabel->setWordWrap(true);
    pathLabel->setStyleSheet("background-color: #f5f5f5; padding: 5px; border: 1px solid #ddd;");
    pathInputLayout->addWidget(pathLabel);
    
    browseBtn = new QPushButton("浏览...");
    browseBtn->setFixedWidth(80);
    pathInputLayout->addWidget(browseBtn);
    
    pathLayout->addLayout(pathInputLayout);
    pathGroup->setLayout(pathLayout);
    mainLayout->addWidget(pathGroup);
    
    // ========== 按钮 ==========
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    
    applyBtn = new QPushButton("应用");
    applyBtn->setFixedWidth(80);
    resetBtn = new QPushButton("重置");
    resetBtn->setFixedWidth(80);
    cancelBtn = new QPushButton("取消");
    cancelBtn->setFixedWidth(80);
    
    buttonLayout->addWidget(applyBtn);
    buttonLayout->addWidget(resetBtn);
    buttonLayout->addWidget(cancelBtn);
    
    mainLayout->addLayout(buttonLayout);
    
    // ========== 信号连接 ==========
    connect(threadSlider, QOverload<int>::of(&QSlider::valueChanged),
            this, [this](int value) {
        threadSpinBox->blockSignals(true);
        threadSpinBox->setValue(value);
        threadSpinBox->blockSignals(false);
        onThreadSliderChanged(value);
    });
    
    connect(threadSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int value) {
        threadSlider->blockSignals(true);
        threadSlider->setValue(value);
        threadSlider->blockSignals(false);
    });
    
    connect(fontSizeSlider, QOverload<int>::of(&QSlider::valueChanged),
            this, [this](int value) {
        fontSizeSpinBox->blockSignals(true);
        fontSizeSpinBox->setValue(value);
        fontSizeSpinBox->blockSignals(false);
        onFontSizeChanged(value);
    });
    
    connect(fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int value) {
        fontSizeSlider->blockSignals(true);
        fontSizeSlider->setValue(value);
        fontSizeSlider->blockSignals(false);
    });
    
connect(fontComboBox, &QFontComboBox::currentFontChanged, this, [=](const QFont &font) {
    QFont previewFont = font;
    previewFont.setPointSize(fontSizeSpinBox->value());
    fontPreviewLabel->setFont(previewFont);
}); // <--- 確保這裡正確閉合
    
    connect(browseBtn, &QPushButton::clicked, this, &SettingsDialog::onBrowseFolder);
    connect(applyBtn, &QPushButton::clicked, this, &SettingsDialog::onApplySettings);
    connect(resetBtn, &QPushButton::clicked, this, &SettingsDialog::onResetSettings);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void SettingsDialog::loadSettings() {
    QSettings settings("YTDownloader", "YTDownloader");
    
    currentThreadCount = settings.value("threadCount", 4).toInt();
    currentFontFamily = settings.value("fontFamily", "Microsoft JhengHei UI").toString();
    currentFontSize = settings.value("fontSize", 10).toInt();
    currentDownloadPath = settings.value("downloadPath", 
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/YTDownloads").toString();
    
    setThreadCount(currentThreadCount);
    setFontFamily(currentFontFamily);
    setFontSize(currentFontSize);
    setDownloadPath(currentDownloadPath);
}

void SettingsDialog::saveSettings() {
    QSettings settings("YTDownloader", "YTDownloader");
    settings.setValue("threadCount", currentThreadCount);
    settings.setValue("fontFamily", currentFontFamily);
    settings.setValue("fontSize", currentFontSize);
    settings.setValue("downloadPath", currentDownloadPath);
    settings.sync();
}

int SettingsDialog::getThreadCount() const { return currentThreadCount; }
QString SettingsDialog::getFontFamily() const { return currentFontFamily; }
int SettingsDialog::getFontSize() const { return currentFontSize; }
QString SettingsDialog::getDownloadPath() const { return currentDownloadPath; }

void SettingsDialog::setThreadCount(int count) {
    currentThreadCount = qBound(1, count, 32);
    threadSlider->blockSignals(true);
    threadSpinBox->blockSignals(true);
    threadSlider->setValue(currentThreadCount);
    threadSpinBox->setValue(currentThreadCount);
    threadSlider->blockSignals(false);
    threadSpinBox->blockSignals(false);
}

void SettingsDialog::setFontFamily(const QString &family) {
    currentFontFamily = family;
    fontComboBox->setCurrentFont(QFont(family));
}

void SettingsDialog::setFontSize(int size) {
    currentFontSize = qBound(8, size, 24);
    fontSizeSlider->blockSignals(true);
    fontSizeSpinBox->blockSignals(true);
    fontSizeSlider->setValue(currentFontSize);
    fontSizeSpinBox->setValue(currentFontSize);
    fontSizeSlider->blockSignals(false);
    fontSizeSpinBox->blockSignals(false);
}

void SettingsDialog::setDownloadPath(const QString &path) {
    currentDownloadPath = path;
    if (path.isEmpty()) {
        pathLabel->setText("未设定");
    } else {
        pathLabel->setText(path);
    }
}

void SettingsDialog::onThreadSliderChanged(int value) {
    currentThreadCount = value;
    QString info;
    if (value >= 1 && value <= 4) {
        info = QString("当前: %1 线程 (低速)").arg(value);
    } else if (value <= 8) {
        info = QString("当前: %1 线程 (推荐)").arg(value);
    } else if (value <= 16) {
        info = QString("当前: %1 线程 (高速)").arg(value);
    } else {
        info = QString("当前: %1 线程 (极速)").arg(value);
    }
    threadInfoLabel->setText(info);
}

void SettingsDialog::onFontSizeChanged(int value) {
    currentFontSize = value;
    QFont previewFont = fontComboBox->currentFont();
    previewFont.setPointSize(value);
    fontPreviewLabel->setFont(previewFont);
}

void SettingsDialog::onBrowseFolder() {
    QString dir = QFileDialog::getExistingDirectory(this, "选择下载位置");
    if (!dir.isEmpty()) {
        setDownloadPath(dir);
    }
}

void SettingsDialog::onApplySettings() {
    currentThreadCount = threadSpinBox->value();
    currentFontFamily = fontComboBox->currentFont().family();
    currentFontSize = fontSizeSpinBox->value();
    
    saveSettings();
    accept();
}

void SettingsDialog::onResetSettings() {
    setThreadCount(4);
    setFontFamily("Microsoft JhengHei UI");
    setFontSize(10);
    setDownloadPath(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/YTDownloads");
}
