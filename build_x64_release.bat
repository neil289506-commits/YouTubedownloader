@echo off
REM ========================================
REM YouTube 下載器 Pro - x64 Release 編譯腳本
REM ========================================

setlocal enabledelayedexpansion

REM 色彩代碼
set RED=[91m
set GREEN=[92m
set YELLOW=[93m
set BLUE=[94m
set RESET=[0m

cls
echo ╔════════════════════════════════════════════════════════════╗
echo ║ YouTube 下載器 Pro v2.0 - CMake x64 Release 編譯        ║
echo ╚════════════════════════════════════════════════════════════╝
echo.

REM ========================================
REM 步驟 1: 檢查環境
REM ========================================
echo [步驟 1/7] 檢查環境...

REM 檢查 CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    echo ❌ CMake 未找到！請安裝 CMake 3.20+
    echo 下載: https://cmake.org/download/
    goto error
)
for /f "tokens=3" %%i in ('cmake --version') do set CMAKE_VER=%%i
echo ✓ CMake: !CMAKE_VER!

REM 檢查 Visual Studio
if not exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    echo ❌ Visual Studio 2022 未找到！
    goto error
)
echo ✓ Visual Studio 2022 已安裝

REM 檢查 Qt
set QTDIR=C:\Qt\6.11.0\msvc2022_64
if not exist "%QTDIR%" (
    echo ❌ Qt 6.11.0 未找到！
    echo 預期位置: %QTDIR%
    echo 提示: 可修改此腳本中的 QTDIR 變數
    goto error
)
echo ✓ Qt 6.11.0: %QTDIR%

echo.

REM ========================================
REM 步驟 2: 設定環境變數
REM ========================================
echo [步驟 2/7] 設定環境變數...

REM 初始化 Visual Studio 環境
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if errorlevel 1 (
    echo ❌ Visual Studio 環境初始化失敗
    goto error
)
echo ✓ Visual Studio 環境已初始化

REM 設定 Qt 相關變數
set PATH=%QTDIR%\bin;%PATH%
set CMAKE_PREFIX_PATH=%QTDIR%
set QT_QPA_PLATFORM_PLUGIN_PATH=%QTDIR%\plugins

echo ✓ Qt 環境變數已設定

echo.

REM ========================================
REM 步驟 3: 清潔舊版本 (可選)
REM ========================================
echo [步驟 3/7] 清潔舊版本...

if exist build (
    echo 清潔構建目錄...
    rmdir /s /q build >nul 2>&1
)

if exist bin (
    echo 清潔輸出目錄...
    rmdir /s /q bin >nul 2>&1
)

echo ✓ 舊版本已清潔

echo.

REM ========================================
REM 步驟 4: 創建構建目錄
REM ========================================
echo [步驟 4/7] 創建構建目錄...

mkdir build
cd build

if errorlevel 1 (
    echo ❌ 無法創建構建目錄
    goto error
)

echo ✓ 構建目錄已創建

echo.

REM ========================================
REM 步驟 5: CMake 配置 (x64)
REM ========================================
echo [步驟 5/7] CMake 配置 (x64)...

cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=%QTDIR% -DCMAKE_BUILD_TYPE=Release

if errorlevel 1 (
    echo ❌ CMake 配置失敗
    echo 查看上面的錯誤信息
    goto error_restore
)

echo ✓ CMake 配置完成

echo.

REM ========================================
REM 步驟 6: 編譯 (Release 模式)
REM ========================================
echo [步驟 6/7] 編譯 (Release 模式)...

cmake --build . --config Release --parallel 4

if errorlevel 1 (
    echo ❌ 編譯失敗
    echo 查看上面的錯誤信息
    goto error_restore
)

echo ✓ 編譯完成

echo.

REM ========================================
REM 步驟 7: 驗證和部署
REM ========================================
echo [步驟 7/7] 驗證和部署...

if not exist "bin\Release\YTDownloader.exe" (
    echo ❌ 可執行文件未生成
    goto error_restore
)

echo ✓ 可執行文件已生成

REM 部署 Qt 依賴
echo 部署 Qt 庫...
%QTDIR%\bin\windeployqt.exe .\bin\Release\YTDownloader.exe >nul 2>&1

echo ✓ Qt 庫已部署

echo.

REM ========================================
REM 編譯成功
REM ========================================
echo.
echo ╔════════════════════════════════════════════════════════════╗
echo ║              ✓ 編譯成功！                                ║
echo ╚════════════════════════════════════════════════════════════╝
echo.

echo 📁 可執行文件位置:
echo    %cd%\bin\Release\YTDownloader.exe
echo.

echo 🚀 開始運行應用:
echo    .\bin\Release\YTDownloader.exe
echo.

echo 📊 編譯信息:
echo    架構:  x64 (64-bit)
echo    模式:  Release (已優化)
echo    編譯器: MSVC 2022
echo    Qt 版本: 6.11.0
echo.

REM 詢問是否運行
set /p RUN="是否立即運行應用? (Y/N): "
if /i "%RUN%"=="Y" (
    echo.
    echo 啟動應用...
    start .\bin\Release\YTDownloader.exe
    echo ✓ 應用已啟動
)

pause
exit /b 0

REM ========================================
REM 錯誤處理
REM ========================================
:error
cd ..
echo.
echo ❌ 編譯失敗！
echo.
echo 疑難排除:
echo   1. 確保所有依賴已安裝
echo   2. 檢查 Qt 路徑: %QTDIR%
echo   3. 運行 Visual Studio Installer 安裝缺失組件
echo   4. 嘗試手動執行 CMAKE_BUILD_GUIDE.md 中的命令
echo.
pause
exit /b 1

:error_restore
cd ..
echo.
echo ❌ 編譯失敗！
echo.
echo 請查看上面的錯誤信息
echo.
pause
exit /b 1
