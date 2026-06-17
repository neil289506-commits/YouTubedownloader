# CMakeLists.txt 詳細解讀

本文檔詳細解釋 CMakeLists.txt 中的每一部分配置。

## 文件結構概覽

```
CMakeLists.txt
├── 1. CMake 最低版本要求
├── 2. x64 架構驗證
├── 3. 項目定義
├── 4. 全局配置
├── 5. 編譯器特定配置
├── 6. Qt 6 依賴
├── 7. 源文件定義
├── 8. 可執行文件創建
├── 9. Qt 庫連接
├── 10. Windows 特定配置
└── 11. 輸出信息
```

---

## 詳細解讀

### 1. CMake 最低版本

```cmake
cmake_minimum_required(VERSION 3.20)
```

**說明**:
- 要求 CMake 版本 3.20 或更高
- 3.20 支持 Qt 6 的完整功能
- 低於此版本會報錯

**檢查版本**:
```bash
cmake --version
# 預期: cmake version 3.20+
```

---

### 2. x64 架構驗證

```cmake
# 設定 x64 架構 (必須在 project() 之前)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "已配置 x64 架構 (64-bit)")
else()
    message(FATAL_ERROR "必須使用 x64 架構！請重新執行 CMake。")
endif()
```

**說明**:
- `CMAKE_SIZEOF_VOID_P` 是指針大小
  - 8 = 64-bit (x64)
  - 4 = 32-bit (x86)
- 此項目強制要求 x64

**強制 x64** (在 cmake 命令中):
```bash
cmake .. -G "Visual Studio 17 2022" -A x64
#                                    ^^^^ 關鍵
```

---

### 3. 項目定義

```cmake
project(YTDownloader
    VERSION 2.0.0
    DESCRIPTION "YouTube 下載器 Pro - Qt 版本"
    LANGUAGES CXX
)
```

**說明**:
- 項目名: `YTDownloader`
- 版本: `2.0.0`
- 只使用 C++ 語言

**使用版本**:
```cmake
# 在代碼中可使用
message(${PROJECT_VERSION})        # 2.0.0
message(${PROJECT_VERSION_MAJOR})  # 2
```

---

### 4. 全局 CMake 配置

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

**說明**:
- **CMAKE_CXX_STANDARD 17**: 使用 C++17 標準
- **CMAKE_CXX_STANDARD_REQUIRED ON**: 強制要求 C++17
- **CMAKE_CXX_EXTENSIONS OFF**: 不使用編譯器特定擴展 (如 GNU C++)

```cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
```

**說明**:
- **AUTOMOC**: 自動運行 Qt Meta-Object Compiler (moc)
- **AUTORCC**: 自動處理 .qrc 資源文件
- **AUTOUIC**: 自動編譯 .ui 文件 (我們沒用)

---

### 5. 輸出目錄配置

```cmake
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
```

**說明**:
- **RUNTIME**: 可執行文件輸出到 `./build/bin`
- **LIBRARY**: 動態庫輸出到 `./build/lib`
- **ARCHIVE**: 靜態庫輸出到 `./build/lib`

**文件結構**:
```
build/
├── bin/
│   ├── Release/
│   │   └── YTDownloader.exe
│   └── Debug/
│       └── YTDownloader.exe
└── lib/
```

---

### 6. MSVC 編譯器配置

```cmake
if(MSVC)
    # MSVC 編譯器配置 (Visual Studio 2022)
    message(STATUS "MSVC 編譯器配置")
    
    set(CMAKE_GENERATOR_PLATFORM "x64")
    
    add_compile_options(
        /W4              # 警告等級 4
        /WX-             # 不將警告視為錯誤
        /utf-8           # UTF-8 源編碼
        /permissive-     # 標準一致性
        /std:c++17       # C++17 標準
    )
```

**說明**:
- **/W4**: 最高警告等級 (推薦用於找 bug)
- **/utf-8**: 源文件編碼為 UTF-8 (支持中文)
- **/permissive-**: 更標準一致的 C++ 編譯
- **/std:c++17**: 明確指定 C++17

```cmake
    # Debug 配置
    add_compile_options($<$<CONFIG:Debug>:/Od /RTC1>)
    
    # Release 配置 (優化)
    add_compile_options($<$<CONFIG:Release>:/O2 /Ob2 /Oi /Ot>)
```

**說明**:
- **/Od**: Debug 時禁用優化 (便於調試)
- **/RTC1**: 運行時檢查
- **/O2**: Release 時啟用最大優化
- **/Ob2**: 內聯函數優化
- **/Oi**: 使用內建函數優化
- **/Ot**: 代碼大小優化

```cmake
    set(CMAKE_EXE_LINKER_FLAGS 
        "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /MACHINE:X64")
```

**說明**:
- **/SUBSYSTEM:WINDOWS**: GUI 應用 (不顯示控制台)
- **/MACHINE:X64**: 明確指定 x64 架構

---

### 7. GCC/Clang 編譯器配置

```cmake
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" 
       OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # GCC/Clang 編譯器配置 (Linux/macOS)
    message(STATUS "GCC/Clang 編譯器配置")
    
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -march=x86-64    # x64 架構
        -O2
    )
```

**說明**:
- **-Wall**: 常見警告
- **-Wextra**: 額外警告
- **-Wpedantic**: 嚴格標準檢查
- **-march=x86-64**: 針對 x64 優化
- **-O2**: 中等優化

---

### 8. Qt 6 依賴

```cmake
find_package(Qt6 COMPONENTS
    Core      REQUIRED
    Gui       REQUIRED
    Widgets   REQUIRED
    Network   REQUIRED
)

message(STATUS "Qt6 版本: ${Qt6_VERSION}")
message(STATUS "Qt6 路徑: ${Qt6_DIR}")
```

**說明**:
- **find_package**: 尋找 Qt6 庫
- **REQUIRED**: 如果找不到會報錯
- 需要的組件:
  - **Core**: 基礎功能 (線程、事件循環)
  - **Gui**: 圖形界面基礎
  - **Widgets**: UI 組件
  - **Network**: 網絡功能

**設定 Qt 路徑**:
```bash
cmake .. -DCMAKE_PREFIX_PATH=C:/Qt/6.11.0/msvc2022_64
```

---

### 9. 源文件定義

```cmake
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
```

**說明**:
- 列出所有源文件
- AUTOMOC 會自動處理頭文件
- AUTORCC 會自動處理 .qrc 文件

---

### 10. 創建可執行文件

```cmake
add_executable(YTDownloader
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_RESOURCES}
)
```

**說明**:
- 創建可執行文件 `YTDownloader`
- 包含所有源文件和資源

---

### 11. 連接 Qt 庫

```cmake
target_link_libraries(YTDownloader
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
)
```

**說明**:
- **PRIVATE**: 庫只供此目標使用
- 連接 Qt 6 的各個組件

---

### 12. Windows API 庫

```cmake
if(WIN32)
    target_link_libraries(YTDownloader
        PRIVATE
            user32.lib      # 用戶界面
            shell32.lib     # Shell API
            ole32.lib       # COM 對象
            oleaut32.lib    # OLE 自動化
            urlmon.lib      # URL Moniker (URLDownloadToFile)
            dwmapi.lib      # 桌面窗口管理器
    )
```

**說明**:
- **user32**: 窗口消息、事件
- **shell32**: 外殼操作 (文件夾等)
- **ole32/oleaut32**: COM 支持
- **urlmon**: 下載功能 (`URLDownloadToFileW`)
- **dwmapi**: 深色主題支持

---

### 13. 包含目錄

```cmake
target_include_directories(YTDownloader
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
```

**說明**:
- 添加源目錄到包含路徑
- 便於 `#include` 查找文件

---

### 14. 輸出信息

```cmake
message(STATUS "")
message(STATUS "╔═══════════════════════════════════════╗")
message(STATUS "║ YouTube 下載器 Pro v2.0 - CMake 配置 ║")
message(STATUS "╚═══════════════════════════════════════╝")
message(STATUS "")
message(STATUS "🏗️  編譯配置:")
message(STATUS "   CMake 版本:        ${CMAKE_VERSION}")
message(STATUS "   編譯器:            ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "   C++ 標準:          ${CMAKE_CXX_STANDARD}")
message(STATUS "   架構:              x64 (64-bit)")
```

**說明**:
- 美化輸出
- 顯示編譯配置信息
- 便於用戶驗證設定

---

## 自訂配置

### 添加編譯選項

```cmake
# 在 CMakeLists.txt 中添加
add_compile_options(-Wall -Wextra)

# 或特定配置
add_compile_options($<$<CONFIG:Release>:-O3>)
```

### 添加新源文件

```cmake
# 修改 PROJECT_SOURCES
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    downloadworker.cpp
    newsource.cpp  # 新增
)
```

### 添加依賴庫

```cmake
# 修改 target_link_libraries
target_link_libraries(YTDownloader
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::Sql  # 新增
)
```

### 啟用 LTO (Link Time Optimization)

```cmake
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
```

---

## 常見 CMake 變數

| 變數 | 值 | 說明 |
|------|-----|------|
| `CMAKE_BINARY_DIR` | 構建目錄路徑 | 通常是 `build/` |
| `CMAKE_SOURCE_DIR` | 源代碼目錄 | 項目根目錄 |
| `CMAKE_CXX_COMPILER` | cl.exe | C++ 編譯器 |
| `CMAKE_BUILD_TYPE` | Debug/Release | 構建模式 |
| `CMAKE_SYSTEM_NAME` | Windows | 操作系統 |
| `MSVC` | 1/0 | 是否使用 MSVC |

---

## 調試 CMake

```bash
# 顯示詳細信息
cmake .. --debug-output

# 列出所有變數
cmake .. -LA

# 特定變數
cmake .. -DCMAKE_MESSAGE_LOG_LEVEL=VERBOSE
```

---

## 最佳實踐

✅ **推薦**:
- 使用 `PRIVATE` 限定符
- 設置 `CMAKE_CXX_STANDARD_REQUIRED ON`
- 在適當位置添加 `message(STATUS ...)`
- 為不同編譯器提供配置

❌ **避免**:
- 硬編碼絕對路徑
- 混合舊式和新式 CMake 語法
- 過度複雜的條件邏輯
- 不必要的全局設置

---

**希望本文檔幫助您理解 CMakeLists.txt！** 🎉
