# CMake x64 編譯 - 快速參考卡片

## 📌 一頁速查表

### 環境設定 (Windows)

```batch
REM 設定 Qt 路徑
set QTDIR=C:\Qt\6.11.0\msvc2022_64
set PATH=%QTDIR%\bin;%PATH%
set CMAKE_PREFIX_PATH=%QTDIR%

REM 初始化 Visual Studio 環境
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
```

### 快速編譯 (3 步)

```bash
# 步驟 1: 進入項目並創建構建目錄
cd YTDownloader
mkdir build && cd build

# 步驟 2: CMake 配置 (x64)
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=%QTDIR%

# 步驟 3: 編譯 (Release)
cmake --build . --config Release
```

### 運行應用

```bash
.\bin\Release\YTDownloader.exe
```

---

## 🎯 常見命令速查

| 任務 | 命令 |
|------|------|
| **配置 Release** | `cmake .. -G "Visual Studio 17 2022" -A x64` |
| **配置 Debug** | `cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug` |
| **編譯 Release** | `cmake --build . --config Release` |
| **編譯 Debug** | `cmake --build . --config Debug` |
| **並行編譯 (4 核)** | `cmake --build . --config Release -j 4` |
| **詳細日誌** | `cmake --build . --verbose` |
| **清潔** | `cmake --build . --target clean` |
| **重新配置** | `rm -rf build && mkdir build && cd build && cmake ..` |

---

## ✅ 驗證清單

### 編譯前
- [ ] CMake 已安裝 (`cmake --version`)
- [ ] Visual Studio 2022 已安裝
- [ ] Qt 6.11.0 MSVC 版本已安裝
- [ ] 環境變數已設定 (`echo %QTDIR%`)

### 編譯中
- [ ] CMake 配置成功 (找到 Qt)
- [ ] 編譯無警告和錯誤
- [ ] 進度條完成到 100%

### 編譯後
- [ ] 可執行文件存在: `.\bin\Release\YTDownloader.exe`
- [ ] 應用可以運行
- [ ] 功能正常

---

## 🔧 故障排查

### 快速診斷

```bash
# 檢查 CMake
cmake --version

# 檢查 Qt
qmake --version

# 檢查編譯器
cl.exe

# 檢查架構
cmake --system-information | findstr "CMAKE_SIZEOF_VOID_P"
# 應該顯示: 8 (表示 x64)
```

### 常見問題速解

| 問題 | 快速解決 |
|------|--------|
| 找不到 Qt | `set CMAKE_PREFIX_PATH=C:\Qt\6.11.0\msvc2022_64` |
| 找不到編譯器 | 運行 `vcvars64.bat` |
| 不是 x64 | 添加 `-A x64` 到 cmake 命令 |
| 編譯失敗 | 運行 `cmake --build . --verbose` 查看詳細錯誤 |

---

## 📝 最小配置文件

如果 CMakeLists.txt 丟失，創建最小配置:

```cmake
cmake_minimum_required(VERSION 3.20)
project(YTDownloader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Network REQUIRED)

add_executable(YTDownloader
    main.cpp mainwindow.cpp downloadworker.cpp
    mainwindow.h downloadworker.h
    resources.qrc
)

target_link_libraries(YTDownloader
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network
    user32.lib shell32.lib ole32.lib oleaut32.lib urlmon.lib dwmapi.lib
)
```

---

## 🎬 一鍵腳本

### PowerShell 版本 (build.ps1)

```powershell
# 設定環境
$QTDIR = "C:\Qt\6.11.0\msvc2022_64"
$env:Path = "$QTDIR\bin;$env:Path"
$env:CMAKE_PREFIX_PATH = $QTDIR

# 編譯
mkdir -Force build | cd
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=$QTDIR
cmake --build . --config Release
.\bin\Release\YTDownloader.exe
```

### Bash 版本 (Linux/macOS) (build.sh)

```bash
#!/bin/bash
set -e

QTDIR="/opt/Qt/6.11.0"
CMAKE_PREFIX_PATH=$QTDIR

mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
./bin/YTDownloader
```

---

## 📊 架構驗證

驗證編譯的是 x64:

```bash
# 使用 dumpbin (Windows)
dumpbin /headers .\bin\Release\YTDownloader.exe | findstr "machine"
# 預期: Machine (0x8664) <- x64

# 使用 file (Linux)
file ./bin/YTDownloader
# 預期: ELF 64-bit
```

---

## 🚀 性能優化建議

### Release 編譯優化

```bash
# 啟用最大優化
cmake .. -DCMAKE_CXX_FLAGS_RELEASE="/O2 /Ob2 /Oi /Ot /arch:AVX2"
```

### Debug 編譯調試

```bash
# 保留符號用於調試
cmake .. -DCMAKE_CXX_FLAGS_DEBUG="/Zi /RTC1 /fsanitize=address"
```

---

## 📚 其他資源

| 文檔 | 用途 |
|------|------|
| `CMAKE_BUILD_GUIDE.md` | 完整編譯指南 |
| `build_x64_release.bat` | 自動化腳本 |
| `CMakeLists.txt` | CMake 配置 |
| `README.md` | 功能說明 |

---

## 💡 提示

✅ **使用自動化腳本** (build_x64_release.bat)  
✅ **檢查環境變數** 設定無誤  
✅ **使用 Release 模式** 以獲得最佳性能  
✅ **使用 `-j 4`** 並行編譯以加快速度  
✅ **運行 windeployqt** 部署 Qt 庫  

❌ **避免**在源代碼目錄構建  
❌ **避免**混合 Debug 和 Release  
❌ **避免**忽略 CMake 警告  

---

**祝您編譯順利！** 🎉
