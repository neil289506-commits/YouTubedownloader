# YouTube 下載器 Pro - CMake x64 編譯指南

使用 CMake 和 x64 架構編譯項目的完整指南。

## 📋 目錄

- [系統要求](#系統要求)
- [快速開始](#快速開始)
- [詳細步驟](#詳細步驟)
- [編譯命令](#編譯命令)
- [常見問題](#常見問題)
- [進階配置](#進階配置)

---

## 系統要求

### 開發環境
- **CMake**: 3.20 或更高版本
- **Visual Studio**: 2022 Community Edition (或更高版本)
- **Qt**: 6.11.0 MSVC 2022 版本 (x64)
- **C++**: C++17 標準支持

### 系統架構
- **強制**: x64 (64-bit) 架構
- **不支持**: x86 (32-bit)

### 安裝工具

#### 1. CMake 安裝
```bash
# 選項 A: 從官方網站下載
https://cmake.org/download/

# 選項 B: 使用 Chocolatey (Windows)
choco install cmake

# 選項 C: 使用 scoop
scoop install cmake

# 驗證安裝
cmake --version
# 應該顯示: cmake version 3.20+ 或更高
```

#### 2. Visual Studio 2022
```bash
# 下載: https://visualstudio.microsoft.com/
# 安裝工作負載:
# - Desktop development with C++
# - MSVC v143 - VS 2022 C++ x64/x86 build tools
```

#### 3. Qt 6.11.0
```bash
# 下載: https://www.qt.io/download

# 重要: 安裝時勾選 MSVC 2022 64-bit 版本
# 安裝路徑: C:\Qt\6.11.0\msvc2022_64\
```

---

## 快速開始

### 一鍵編譯 (5 分鐘)

#### Windows (MSVC 2022)

```bash
# 1. 進入項目目錄
cd path\to\YTDownloader

# 2. 設定 Qt 環境
set QTDIR=C:\Qt\6.11.0\msvc2022_64
set PATH=%QTDIR%\bin;%PATH%

# 3. 創建並進入構建目錄
mkdir build && cd build

# 4. 生成 Visual Studio 項目 (x64)
cmake .. -G "Visual Studio 17 2022" -A x64

# 5. 編譯
cmake --build . --config Release

# 6. 運行
.\bin\Release\YTDownloader.exe
```

#### Linux (GCC/Clang)

```bash
# 1. 安裝依賴
sudo apt-get install cmake build-essential qt6-base-dev qt6-tools-dev

# 2. 創建構建目錄
mkdir build && cd build

# 3. 生成 Makefile
cmake .. -DCMAKE_BUILD_TYPE=Release

# 4. 編譯
cmake --build . -j$(nproc)

# 5. 運行
./bin/YTDownloader
```

---

## 詳細步驟

### 步驟 1: 驗證環境

```bash
# 檢查 CMake
cmake --version
# 預期: cmake version 3.20 或更高

# 檢查 Visual Studio
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

# 檢查 Qt
%QTDIR%\bin\qmake.exe --version
# 預期: QMake version 3.1 using Qt version 6.11.0
```

### 步驟 2: 設定環境變數 (Windows)

創建文件 `setup_env.bat`:

```batch
@echo off
REM 設定 Qt 路徑
set QTDIR=C:\Qt\6.11.0\msvc2022_64
set PATH=%QTDIR%\bin;%PATH%

REM 設定 CMake
set CMAKE_PREFIX_PATH=%QTDIR%

REM 設定 Visual Studio 2022
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

REM 驗證
echo Qt 路徑: %QTDIR%
echo CMake 前綴路徑: %CMAKE_PREFIX_PATH%
echo.
echo 環境變數已設定！
pause
```

執行:
```bash
setup_env.bat
```

### 步驟 3: 配置項目 (CMake)

```bash
# 進入項目目錄
cd YTDownloader

# 創建構建目錄
mkdir build
cd build

# 生成 Visual Studio 2022 x64 項目
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=%QTDIR%

# 或 Release 模式
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
```

**預期輸出**:
```
CMake 版本 3.27+
-- 系統: Windows
-- 編譯器: MSVC
-- 架構: x64
-- Qt6 找到: C:/Qt/6.11.0/msvc2022_64/
-- 已配置 x64 架構 (64-bit)

╔═══════════════════════════════════════╗
║ YouTube 下載器 Pro v2.0 - CMake 配置 ║
╚═══════════════════════════════════════╝

編譯配置:
   CMake 版本:        3.27+
   編譯器:            MSVC 19.3x
   C++ 標準:          17
   架構:              x64 (64-bit)

依賴:
   Qt6 版本:          6.11.0
```

### 步驟 4: 編譯

#### Release 版本 (優化)

```bash
# 使用多核編譯
cmake --build . --config Release -j 4

# 或簡單方式
cmake --build . --config Release
```

#### Debug 版本 (調試)

```bash
cmake --build . --config Debug
```

### 步驟 5: 運行

```bash
# Release 版本
.\bin\Release\YTDownloader.exe

# Debug 版本
.\bin\Debug\YTDownloader.exe
```

---

## 編譯命令

### 完整編譯流程

```bash
REM ========================================
REM 1. 設定環境
REM ========================================
set QTDIR=C:\Qt\6.11.0\msvc2022_64
set PATH=%QTDIR%\bin;%PATH%
set CMAKE_PREFIX_PATH=%QTDIR%

REM ========================================
REM 2. 清潔舊版本 (可選)
REM ========================================
if exist build rmdir /s /q build
if exist bin rmdir /s /q bin

REM ========================================
REM 3. 創建構建目錄
REM ========================================
mkdir build
cd build

REM ========================================
REM 4. CMake 配置 (生成 Visual Studio 項目)
REM ========================================
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=%QTDIR%

REM ========================================
REM 5. 編譯 (Release 模式)
REM ========================================
cmake --build . --config Release

REM ========================================
REM 6. 部署 (複製 Qt DLL)
REM ========================================
%QTDIR%\bin\windeployqt.exe .\bin\Release\YTDownloader.exe

REM ========================================
REM 7. 運行
REM ========================================
.\bin\Release\YTDownloader.exe
```

保存為 `build.bat`:

```bash
call build.bat
```

### 簡化編譯命令

```bash
# 一鍵編譯 (在項目目錄執行)
set QTDIR=C:\Qt\6.11.0\msvc2022_64 && set CMAKE_PREFIX_PATH=%QTDIR% && mkdir build && cd build && cmake .. -G "Visual Studio 17 2022" -A x64 && cmake --build . --config Release && cd bin\Release && YTDownloader.exe
```

### CMake 高級選項

```bash
# 詳細構建日誌
cmake --build . --config Release --verbose

# 並行編譯 (4 核)
cmake --build . --config Release -j 4

# 指定生成器
cmake .. -G "Ninja Multi-Config"

# 編譯特定目標
cmake --build . --target YTDownloader --config Release

# 清潔構建
cmake --build . --target clean
cmake --build . --config Release
```

---

## 常見問題

### Q1: "找不到 Qt"

**錯誤信息**:
```
CMake Error at CMakeLists.txt:XX (find_package):
  Could not find Qt6 library
```

**解決方案**:

```bash
# 設定 CMAKE_PREFIX_PATH
set CMAKE_PREFIX_PATH=C:\Qt\6.11.0\msvc2022_64

# 重新運行 CMake
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%
```

或在 CMakeLists.txt 中添加:

```cmake
set(Qt6_DIR "C:/Qt/6.11.0/msvc2022_64/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH "C:/Qt/6.11.0/msvc2022_64")
```

### Q2: "不是 x64 架構"

**錯誤信息**:
```
CMake Error: Must use x64 architecture!
```

**解決方案**:

```bash
# 確保使用 x64 生成器
cmake .. -G "Visual Studio 17 2022" -A x64
#                                    ^^^^^^ 重要!

# 驗證架構
cmake --system-information | find "CMAKE_SIZEOF_VOID_P"
# 應該顯示: 8 (表示 64-bit)
```

### Q3: "找不到 MSVC 編譯器"

**錯誤信息**:
```
CMake Error: No CMAKE_CXX_COMPILER could be found
```

**解決方案**:

```bash
# 方法 1: 使用 vcvars64.bat 初始化環境
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

# 方法 2: 指定編譯器路徑
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_CXX_COMPILER="cl.exe"
```

### Q4: "編譯失敗 - 找不到 Windows.h"

**解決方案**:

確保已安裝 Windows SDK:
```bash
# 在 Visual Studio Installer 中:
Community → Modify → 
  Individual Components →
    Windows 10/11 SDK 搜尋並勾選
```

### Q5: "URLDownloadToFileW 找不到"

**錯誤**:
```
LNK2019: unresolved external symbol URLDownloadToFileW
```

**解決方案**:

確保 CMakeLists.txt 包含:
```cmake
target_link_libraries(YTDownloader PRIVATE urlmon.lib)
```

已經在提供的 CMakeLists.txt 中包含。

### Q6: "無法找到 Qt6Config.cmake"

**解決方案**:

```bash
# 檢查 Qt6 安裝
dir C:\Qt\6.11.0\msvc2022_64\lib\cmake\Qt6

# 如果不存在，設定完整路徑
cmake .. -DQt6_DIR=C:/Qt/6.11.0/msvc2022_64/lib/cmake/Qt6
```

---

## 進階配置

### 自定義編譯選項

編輯 CMakeLists.txt:

```cmake
# 啟用 LTO (Link Time Optimization)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

# 自定義優化級別
add_compile_options($<$<CONFIG:Release>:/O2 /Ob2>)

# 啟用 Address Sanitizer (Debug 版本)
add_compile_options($<$<CONFIG:Debug>:/fsanitize=address>)
```

### 靜態連接 Qt

```cmake
# 在 CMakeLists.txt 中
set(CMAKE_PREFIX_PATH "C:/Qt/6.11.0/msvc2022_64/lib/cmake")

# 靜態連接 Qt
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED STATIC)
```

### 產生 Visual Studio 解決方案

```bash
# 而不是命令行編譯，直接打開 Visual Studio
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=%QTDIR%

# 生成 YTDownloader.sln
# 用 Visual Studio 打開並編譯
```

### 跨編譯器配置

```cmake
# 檢測編譯器
if(MSVC)
    message(STATUS "使用 MSVC 編譯器")
    add_compile_options(/W4)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "使用 GCC 編譯器")
    add_compile_options(-Wall -Wextra)
endif()
```

---

## x64 架構驗證

### 驗證編譯結果

```bash
# 使用 dumpbin 查看可執行文件架構
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.3x.xxxxx\bin\Hostx64\x64\dumpbin.exe" /headers .\bin\Release\YTDownloader.exe | findstr "machine"

# 預期輸出:
#   Machine (0x8664)      <- 表示 x64
```

### x64 特定優化

```cmake
# 在 CMakeLists.txt 中
if(MSVC)
    # x64 特定編譯選項
    add_compile_options(
        /arch:AVX2        # 使用 AVX2 指令集
        /fp:fast          # 快速浮點運算
    )
endif()
```

---

## 構建系統對比

| 特性 | qmake | CMake |
|------|-------|-------|
| **易用性** | 簡單 | 複雜 |
| **靈活性** | 低 | 高 |
| **IDE 支持** | Qt Creator | 各種 IDE |
| **跨平台** | 好 | 更好 |
| **編譯速度** | 中等 | 快 |
| **x64 支持** | 自動 | 需手動 |

---

## 故障排除流程

```
編譯失敗?
   ↓
1. 檢查 CMake 輸出信息 (查找 ERROR)
   ↓
2. 驗證環境變數 (QTDIR, CMAKE_PREFIX_PATH)
   ↓
3. 確認 x64 生成器 (-A x64)
   ↓
4. 檢查編譯器路徑
   ↓
5. 清潔重建 (rm -rf build, mkdir build)
   ↓
6. 查看完整日誌 (cmake --build . --verbose)
   ↓
7. 檢查本文檔的「常見問題」部分
```

---

## 最佳實踐

### ✅ 推薦做法

1. **使用清潔的構建目錄**
   ```bash
   rm -rf build && mkdir build && cd build
   cmake ..
   ```

2. **總是指定架構**
   ```bash
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```

3. **設定 Qt 路徑**
   ```bash
   -DCMAKE_PREFIX_PATH=C:/Qt/6.11.0/msvc2022_64
   ```

4. **使用 Release 模式優化**
   ```bash
   cmake --build . --config Release
   ```

5. **驗證依賴**
   ```bash
   cmake .. --debug-output
   ```

### ❌ 避免做法

1. ❌ 混合 32-bit 和 64-bit
2. ❌ 不設定 CMAKE_PREFIX_PATH
3. ❌ 在源代碼目錄構建 (原地構建)
4. ❌ 使用過舊的 CMake 版本
5. ❌ 忽略 CMake 的警告

---

## 清單

編譯前檢查:
- [ ] CMake 已安裝 (3.20+)
- [ ] Visual Studio 2022 已安裝
- [ ] Qt 6.11.0 MSVC x64 已安裝
- [ ] 環境變數已設定
- [ ] 源代碼文件完整

編譯後驗證:
- [ ] 編譯無錯誤
- [ ] 生成 YTDownloader.exe
- [ ] 應用正常運行
- [ ] 所有功能可用
- [ ] 架構為 x64

---

## 下一步

✅ 編譯成功？
→ 按照 **README.md** 使用應用

❌ 編譯失敗？
→ 查看「常見問題」部分或「故障排除流程」

🔧 需要自訂？
→ 參考「進階配置」部分

---

**祝您編譯順利！** 🎉

需要幫助? 查看其他文檔:
- `README.md` - 功能說明
- `COMPILATION.md` - 詳細編譯指南 (qmake)
- `PROJECT_SUMMARY.md` - 技術總結
