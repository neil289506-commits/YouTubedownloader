cmake_minimum_required(VERSION 3.20)

# ========================================
# 專案基本配置
# ========================================
project(YTDownloader
    VERSION 2.0.0
    DESCRIPTION "YouTube 下載器 Pro - Qt 版本"
    LANGUAGES CXX
)

# 確保在 project() 偵測完編譯器後才進行 x64 架構檢查
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "已配置 x64 架構 (64-bit)")
else()
    message(FATAL_ERROR "必須使用 x64 架構！請重新執行 CMake。")
endif()

# ========================================
# CMake 全局配置
# ========================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt Automoc, Autorcc, Autouic 自動化工具
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 輸出目錄設定
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# ========================================
# 編譯器特定配置
# ========================================
if(MSVC)
    message(STATUS "MSVC 編譯器配置")
    
    # x64 架構標誌
    set(CMAKE_GENERATOR_PLATFORM "x64")
    
    # 基礎通用編譯選項
    add_compile_options(
        /W4              # 警告等級 4
        /WX-             # 不將警告視為錯誤
        /utf-8           # UTF-8 源編碼
        /permissive-     # 標準一致性
        /std:c++17       # C++17 標準
    )
    
    # 根據不同的構建類型分開配置，防止 /O2 與 /RTC1 衝突
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "配置 Debug 編譯選項 (/Od /RTC1)")
        add_compile_options(/Od /RTC1)
    else()
        message(STATUS "配置 Release 優化編譯選項 (/O2 /Ob2 /Oi /Ot)")
        add_compile_options(/O2 /Ob2 /Oi /Ot)
    endif()
    
    # 連接器選項
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /MACHINE:X64")
    
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # GCC/Clang 編譯器配置 (Linux/macOS)
    message(STATUS "GCC/Clang 編譯器配置")
    
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -march=x86-64    # x64 架構
        -O2
    )
else()
    message(WARNING "未知的編譯器: ${CMAKE_CXX_COMPILER_ID}")
endif()

# ========================================
# Qt 6 依賴
# ========================================
find_package(Qt6 COMPONENTS
    Core      REQUIRED
    Gui       REQUIRED
    Widgets   REQUIRED
    Network   REQUIRED
)

message(STATUS "Qt6 版本: ${Qt6_VERSION}")
message(STATUS "Qt6 路徑: ${Qt6_DIR}")

# ========================================
# 源文件和頭文件
# ========================================
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    downloadworker.cpp
)

set(PROJECT_HEADERS
    mainwindow.h
    downloadworker.h
)

set(PROJECT_RESOURCES
    resources.qrc
)

# ========================================
# 創建可執行文件
# ========================================
add_executable(YTDownloader
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_RESOURCES}
)

# ========================================
# 連接 Qt 庫
# ========================================
target_link_libraries(YTDownloader
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
)

# ========================================
# Windows 特定配置與核心 API 連結
# ========================================
if(WIN32)
    message(STATUS "Windows 平台檢測")
    
    # 連結 Windows 系統內建 API 庫
    target_link_libraries(YTDownloader
        PRIVATE
            user32.lib      # 用戶界面
            shell32.lib     # Shell API
            ole32.lib       # COM 對象
            oleaut32.lib    # OLE 自動化
            urlmon.lib      # URL Moniker (隨附基礎下載功能)
            dwmapi.lib      # 桌面窗口管理器
    )
    
    # 應用程序圖標 (當根目錄存在 app.ico 時，自動引入 app.rc)
    if(EXISTS "${CMAKE_SOURCE_DIR}/app.ico")
        message(STATUS "檢測到 app.ico，啟用圖標資源編譯")
        target_sources(YTDownloader PRIVATE app.rc)
    endif()
    
    # 在非 Debug 下隱藏主控台視窗 (GUI 模式)
    if(MSVC)
        set_target_properties(YTDownloader PROPERTIES
            WIN32_EXECUTABLE ON
        )
    endif()
endif()

# ========================================
# 包含目錄設定
# ========================================
target_include_directories(YTDownloader
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ========================================
# 構建完成輸出信息
# ========================================
message(STATUS "")
message(STATUS "╔═══════════════════════════════════════╗")
message(STATUS "║ YouTube 下載器 Pro v2.0 - CMake 配置 ║")
message(STATUS "╚═══════════════════════════════════════╝")
message(STATUS "")
message(STATUS "🏗️  編譯配置:")
message(STATUS "   CMake 版本:        ${CMAKE_VERSION}")
message(STATUS "   編譯器:            ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "   C++ 標準:          ${CMAKE_CXX_STANDARD}")
message(STATUS "   架構:              x64 (64-bit)")
message(STATUS "")
message(STATUS "📦 依賴:")
message(STATUS "   Qt6 版本:          ${Qt6_VERSION}")
message(STATUS "   Qt6 路徑:          ${Qt6_DIR}")
message(STATUS "")
message(STATUS "📁 路徑:")
message(STATUS "   源代碼目錄:        ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "   構建目錄:          ${CMAKE_BINARY_DIR}")
message(STATUS "   輸出目錄:          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "")
message(STATUS "⚙️  構建命令:")
if(MSVC)
    message(STATUS "   cmake .. -DCMAKE_BUILD_TYPE=Release")
    message(STATUS "   cmake --build . --config Release")
else()
    message(STATUS "   cmake .. -DCMAKE_BUILD_TYPE=Release")
    message(STATUS "   cmake --build . -j$(nproc)")
endif()
message(STATUS "")