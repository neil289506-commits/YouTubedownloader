# =====================================================================
# YouTube 下載器 Pro v2.0.1 - Qt Project File
# 適合發布至 GitHub，支援 QMake 與 Qt Creator 編譯
# =====================================================================

QT       += core gui widgets

CONFIG   += c++17

# 防止 MSVC 編譯時出現中文亂碼，強制核心使用 UTF-8
win32-msvc {
    QMAKE_CXXFLAGS += /utf-8 /permissive-
}

TARGET = YTDownloader
TEMPLATE = app

# 預設定義，確保 Windows 平台不會彈出無用的 CMD 黑視窗
win32 {
    CONFIG += windows
}

# 核心原始碼架構定義
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    downloadworker.cpp \
    settingsdialog.cpp

HEADERS += \
    mainwindow.h \
    downloadworker.h \
    settingsdialog.h

# 💡 記憶體內部資源檔（用於 Qt 代碼內讀取樣式、QIcon 資源）
RESOURCES += \
    resources.qrc

# 💡 Windows 系統專屬資源檔（用於設定產出的 .exe 外部圖標與右鍵詳細版本資訊）
win32 {
    RC_FILE = version.rc
}

# 配置編譯輸出目錄，使 build 區塊乾淨整齊
DESTDIR = $$PWD/build/bin
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
RCC_DIR = $$PWD/build/rcc
UI_DIR = $$PWD/build/ui

# GitHub 部署提示信息
message("=====================================================")
message("📦 YouTube 下載器 Pro - QMake 專案配置成功")
message("🏗️ 平台架構: Windows x64 / C++17 標準")
message("🎨 已成功載入 Windows 外殼圖標資源檔 version.rc")
message("=====================================================")