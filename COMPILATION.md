# YouTube 下載器 Pro - 詳細編譯指南

## 環境準備

### 第一步：安裝 Visual Studio 2022

1. 從 Microsoft 官方網站下載 Visual Studio 2022 Community Edition
   - https://visualstudio.microsoft.com/

2. 安裝時選擇以下工作負載：
   - **Desktop development with C++**
   - **Universal Windows Platform development** (可選)

3. 選擇以下單個組件：
   - **MSVC v143 - VS 2022 C++ x64/x86 build tools**
   - **CMake tools for Windows**
   - **C++ AddressSanitizer**

### 第二步：安裝 Qt 6.11.0

1. 從 Qt 官方網站下載線上安裝程序
   - https://www.qt.io/download

2. 選擇「Open Source」版本

3. 在安裝器中：
   - 登錄或建立 Qt 帳戶
   - 選擇 Qt 6.11.0
   - **重要**: 勾選 `MSVC 2022 64-bit`
   - 不需要勾選 WebAssembly 或 Android

4. 安裝位置建議：`C:\Qt\6.11.0\`

5. 同時安裝 Qt Creator (IDE)

### 第三步：安裝 Python (可選但推薦)

- 下載 Python 3.10+ 並新增至 PATH
- 用於某些高級自動化腳本

## 編譯方法 A：使用 Qt Creator (推薦)

### 步驟 1：開啟項目

```
1. 啟動 Qt Creator
2. 菜單: File → Open File or Project
3. 選擇 YTDownloader.pro
```

### 步驟 2：配置 Kit

```
1. 左側項目樹 → 點擊「Projects」
2. 在「Build & Run」下選擇 Desktop Qt 6.11.0 MSVC
3. 如果沒有自動選擇，手動配置：
   - Compiler: MSVC 2022 (x64)
   - Qt version: 6.11.0 MSVC 2022 64-bit
```

### 步驟 3：配置建構步驟

```
1. 在 Build Settings 中：
   - Build directory: ./build
   - Shadow build: 啟用 (推薦)
   - Qt 會自動配置 qmake 和 make 工具
```

### 步驟 4：編譯

```
菜單: Build → Build Project "YTDownloader"
或按快鍵: Ctrl + B
```

### 步驟 5：運行

```
菜單: Build → Run
或按快鍵: Ctrl + R
```

## 編譯方法 B：使用命令列

### 前置工作

打開 Visual Studio 2022 Command Prompt (x64):

```bash
# 在 Windows 開始菜單搜索「Developer Command Prompt」
# 選擇 "Developer Command Prompt for VS 2022"
```

### 設定環境變數

```batch
# 設定 Qt 路徑
set QTDIR=C:\Qt\6.11.0\msvc2022_64
set PATH=%QTDIR%\bin;%PATH%

# 驗證 qmake
qmake --version
# 應該顯示: QMake version 3.1 using Qt version 6.11.0
```

### 編譯步驟

```batch
# 進入項目目錄
cd path\to\YTDownloader

# 創建構建目錄
mkdir build
cd build

# 運行 qmake
qmake ..\YTDownloader.pro -spec win32-msvc

# 編譯
nmake
```

### 運行可執行文件

```batch
# Release 版本
release\YTDownloader.exe

# 或 Debug 版本
debug\YTDownloader.exe
```

## 編譯方法 C：使用 CMake (進階)

如果想使用 CMake，可以建立 `CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.20)
project(YTDownloader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Network REQUIRED)

add_executable(YTDownloader
    main.cpp
    mainwindow.h mainwindow.cpp
    downloadworker.h downloadworker.cpp
    resources.qrc
)

target_link_libraries(YTDownloader
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)
```

編譯命令：

```batch
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

## 常見錯誤排查

### 錯誤 1: "qmake: command not found"

**原因**: Qt 路徑未添加到 PATH

**解決**:
```batch
# 使用完整路徑運行 qmake
C:\Qt\6.11.0\msvc2022_64\bin\qmake.exe ..\YTDownloader.pro
```

### 錯誤 2: "Cannot find -luser32"

**原因**: 連接器無法找到 Windows SDK 庫

**解決**:
```batch
# 確保使用了 Visual Studio 命令提示符
# 或在 pro 文件中明確指定:
LIBS += -luser32 -lshell32 -lole32 -loleaut32
```

### 錯誤 3: "LNK2019 unresolved external symbol"

**原因**: 缺少 pragma comment 庫聲明

**解決**:
在 downloadworker.cpp 中確認包含：
```cpp
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ole32.lib")
```

### 錯誤 4: Qt 版本不匹配

**原因**: 編譯器和 Qt 版本不一致

**驗證**:
```batch
# 檢查 qmake 版本
qmake --version

# 應該顯示: QMake version 3.1 using Qt version 6.11.0
```

### 錯誤 5: "moc: could not exec..."

**原因**: Qt 工具路徑不正確

**解決**:
在 Qt Creator 中：
```
Tools → Options → Kits
檢查 MSVC 2022 工具鏈的路徑設置
```

## 最終化編譯

### 生成 Release 版本

```batch
# 使用 Release 配置編譯
qmake ..\YTDownloader.pro -spec win32-msvc CONFIG+=release
nmake release
```

### 部署應用程序

```batch
# 複製必要的 Qt DLL
C:\Qt\6.11.0\msvc2022_64\bin\windeployqt.exe .\release\YTDownloader.exe
```

此命令會自動複製所有必要的 Qt 庫到應用目錄。

### 結果

編譯後的可執行文件位置：
```
YTDownloader\build\release\YTDownloader.exe
```

可以直接在任何 Windows 10/11 x64 系統上運行。

## 驗證編譯成功

運行應用程序：

```bash
./release/YTDownloader.exe
```

應該看到：
- ✓ 應用窗口打開
- ✓ 深色 UI 主題
- ✓ 所有控件正常
- ✓ 初始化消息出現在日誌中

## 進階設置

### 啟用優化

在 `.pro` 文件中添加：

```qmake
CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS_RELEASE += -O2 -march=native
}
```

### 啟用 Address Sanitizer

```qmake
CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -fsanitize=address
    QMAKE_LFLAGS += -fsanitize=address
}
```

### 跨平台支持 (未來)

若要編譯 Linux 或 macOS 版本，需要：
- 安裝相應平台的 Qt 版本
- 調整 `downloadworker.cpp` 中的平台特定代碼
- 使用相應平台的編譯器

## 疑難排解資源

- **Qt 文檔**: https://doc.qt.io/qt-6/
- **Qt 社區論壇**: https://forum.qt.io/
- **Stack Overflow**: 標籤 `qt` 和 `c++`
- **Qt 內置幫助**: Qt Creator → Help → Index

## 快速參考

```batch
# 完整編譯流程
set QTDIR=C:\Qt\6.11.0\msvc2022_64
set PATH=%QTDIR%\bin;%PATH%
cd YTDownloader
mkdir build
cd build
qmake ..\YTDownloader.pro -spec win32-msvc
nmake
release\YTDownloader.exe
```

---

**希望編譯順利！如有問題，請檢查上述排查部分。**
