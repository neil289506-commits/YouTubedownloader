@echo off
REM ========================================
REM YouTube 下載器 Pro - x64 Debug 編譯腳本
REM ========================================

setlocal enabledelayedexpansion

cls
echo ╔════════════════════════════════════════════════════════════╗
echo ║ YouTube 下載器 Pro v2.0 - CMake x64 Debug 編譯          ║
echo ╚════════════════════════════════════════════════════════════╝
echo.

REM ========================================
REM 環境設定
REM ========================================
echo [初始化環境...]

set QTDIR=C:\Qt\6.11.0\msvc2022_64

REM Visual Studio 環境
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1

REM Qt 環境
set PATH=%QTDIR%\bin;%PATH%
set CMAKE_PREFIX_PATH=%QTDIR%

echo ✓ 環境已初始化
echo.

REM ========================================
REM 清潔和創建構建目錄
REM ========================================
echo [創建構建目錄...]

if exist build_debug rmdir /s /q build_debug
mkdir build_debug
cd build_debug

echo ✓ 構建目錄已創建
echo.

REM ========================================
REM CMake 配置 (Debug)
REM ========================================
echo [CMake 配置...]

cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=%QTDIR% -DCMAKE_BUILD_TYPE=Debug

if errorlevel 1 (
    echo ❌ CMake 配置失敗
    pause
    exit /b 1
)

echo ✓ CMake 配置完成
echo.

REM ========================================
REM 編譯 (Debug 模式)
REM ========================================
echo [編譯 Debug 版本...]

cmake --build . --config Debug --parallel 4

if errorlevel 1 (
    echo ❌ 編譯失敗
    pause
    exit /b 1
)

echo ✓ 編譯完成
echo.

REM ========================================
REM 完成
REM ========================================
echo ╔════════════════════════════════════════════════════════════╗
echo ║              ✓ Debug 版本編譯成功！                      ║
echo ╚════════════════════════════════════════════════════════════╝
echo.

echo 📁 可執行文件: .\bin\Debug\YTDownloader.exe
echo.

set /p RUN="是否立即運行? (Y/N): "
if /i "%RUN%"=="Y" (
    start .\bin\Debug\YTDownloader.exe
)

pause
cd ..
