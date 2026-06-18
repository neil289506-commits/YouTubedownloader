cmake_minimum_required(VERSION 3.20)

# =====================================================================
# YouTube 下載器 Pro v2.0.1 - 現代化 CMake 配置檔
# 適合發布至 GitHub，支援 Visual Studio、CLion 與 CMake 命令行編譯
# =====================================================================

# 💡 關鍵點：LANGUAGES 必須包含 CXX 和 RC，才能讓 MSVC 編譯 .rc 檔案
project(YTDownloader
    VERSION 2.0.1
    DESCRIPTION "YouTube 下載器 Pro - 支援字體調整與多線程"
    LANGUAGES CXX RC
)

# 限制與驗證必須為 64 位元環境
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "❌ 本專案必須使用 x64 架構！請重新檢查編編環境。")
endif()

# =====================================================================
# CMake 全局建構配置
# =====================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 啟用 Qt 特有的自動化工具
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 統整輸出路徑（對齊 QMake，產出到 build/bin 下）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# 尋找 Qt6 核心套件
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# =====================================================================
# 收集專案所有核心原始碼與資源
# =====================================================================
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    downloadworker.cpp
    downloadworker.h
    settingsdialog.cpp
    settingsdialog.h
    resources.qrc
)

# 💡 關鍵點：如果是 Windows 平台，將 version.rc 當作原始碼一併編入執行檔中
if(WIN32)
    list(APPEND PROJECT_SOURCES version.rc)
endif()

# 建立目標二進位制檔
add_executable(YTDownloader ${PROJECT_SOURCES})

# =====================================================================
# 編譯器與連結器特定配置 (MSVC)
# =====================================================================
if(MSVC)
    add_compile_options(
        /W4
        /utf-8
        /permissive-
        /std:c++17
    )
    
    # 確保發布 Release 時為純 GUI 應用程式，不彈出無用的後台 CMD 黑框
    set_target_properties(YTDownloader PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# 連結 Qt6 系統動態庫
target_link_libraries(YTDownloader PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# =====================================================================
# GitHub 終端建構狀態輸出
# =====================================================================
message(STATUS "")
message(STATUS "╔═════════════════════════════════════════════════════╗")
message(STATUS "║ YouTube 下載器 Pro v2.0.1 - CMake 驗證成功          ║")
message(STATUS "║ 已成功配置 Windows 系統二進位制外殼圖標與版本資訊注入 ║")
message(STATUS "╚═════════════════════════════════════════════════════╝")
message(STATUS "")