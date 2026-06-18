#pragma once

#include <QDialog>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QFontComboBox>
#include <QSlider>
#include <QLabel>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    // 獲取設定值
    int getThreadCount() const;
    QString getFontFamily() const;
    int getFontSize() const;
    QString getDownloadPath() const;

    // 設定初始值
    void setThreadCount(int count);
    void setFontFamily(const QString &family);
    void setFontSize(int size);
    void setDownloadPath(const QString &path);

private slots:
    void onApplySettings();
    void onResetSettings();
    void onBrowseFolder();
    void onThreadSliderChanged(int value);
    void onFontSizeChanged(int value);

private:
    void setupUI();
    void loadSettings();
    void saveSettings();

    // UI 組件 - 線程設置
    QSlider *threadSlider;
    QSpinBox *threadSpinBox;
    QLabel *threadInfoLabel;

    // UI 組件 - 字體設置
    QFontComboBox *fontComboBox;
    QSlider *fontSizeSlider;
    QSpinBox *fontSizeSpinBox;
    QLabel *fontPreviewLabel;

    // UI 組件 - 路徑設置
    QPushButton *browseBtn;
    QLabel *pathLabel;

    // UI 組件 - 按鈕
    QPushButton *applyBtn;
    QPushButton *resetBtn;
    QPushButton *cancelBtn;

    // 本地儲存
    int currentThreadCount;
    QString currentFontFamily;
    int currentFontSize;
    QString currentDownloadPath;
};
