#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <commctrl.h>
#include <string>
#include <filesystem>
#include <thread>
#include <urlmon.h>
#include <dwmapi.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "dwmapi.lib")

namespace fs = std::filesystem;

#define ID_BTN_ADD 101
#define ID_BTN_START 102
#define ID_LV_URLS 103
#define WM_INIT_DONE (WM_USER + 1)

HWND hMainWnd, hUrlInput, hPathInput, hLogBox, hResCombo, hFormatCombo, hFPSCombo, hPlaylistCheck, hLVUrls, hSplash;
HBRUSH g_hbrBlack = CreateSolidBrush(RGB(30, 30, 30));
HBRUSH g_hbrEdit  = CreateSolidBrush(RGB(50, 50, 50));
COLORREF g_txtCol = RGB(240, 240, 240);
COLORREF g_bgCol  = RGB(30, 30, 30);

void DownloadProcess() {
    wchar_t path[1024], res[20], fmt[20], fps[20];
    GetWindowTextW(hPathInput, path, 1024);
    GetWindowTextW(hResCombo, res, 20);
    GetWindowTextW(hFormatCombo, fmt, 20);
    GetWindowTextW(hFPSCombo, fps, 20);
    bool isPlaylist = (SendMessageW(hPlaylistCheck, BM_GETCHECK, 0, 0) == BST_CHECKED);

    int itemCount = ListView_GetItemCount(hLVUrls);
    for (int i = 0; i < itemCount; i++) {
        if (ListView_GetCheckState(hLVUrls, i)) {
            wchar_t url[1024];
            ListView_GetItemText(hLVUrls, i, 0, url, 1024);
            std::wstring resNum = std::wstring(res).substr(0, std::wstring(res).find(L"p"));
            std::wstring cmdStr = L"yt-dlp.exe " + std::wstring(isPlaylist ? L"--yes-playlist" : L"--no-playlist") + L" ";
            if (std::wstring(fmt) == L"mp3" || std::wstring(fmt) == L"mpa") {
                cmdStr += L"-x --audio-format " + std::wstring(fmt) + L" ";
            } else {
                cmdStr += L"-f \"bestvideo[height<=" + resNum + L"][fps<=" + std::wstring(fps) + L"]+bestaudio/best\" --merge-output-format " + std::wstring(fmt) + L" ";
            }
            cmdStr += L"-o \"" + std::wstring(path) + L"/%(title)s.%(ext)s\" " + url;
            _wsystem(cmdStr.c_str());
        }
    }
}

void InitializationThread() {
    // 下載 yt-dlp
    if (!fs::exists("yt-dlp.exe")) URLDownloadToFileW(NULL, L"https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe", L"yt-dlp.exe", 0, NULL);

    // 處理 FFmpeg (包含解壓、搜尋、移動、清理)
    if (!fs::exists("ffmpeg.exe") || !fs::exists("ffprobe.exe")) {
        std::wstring zip = L"ffmpeg_temp.zip";
        std::wstring tempDir = L"ffmpeg_temp_dir";
        if (SUCCEEDED(URLDownloadToFileW(NULL, L"https://github.com/GyanD/codexffmpeg/releases/download/6.0/ffmpeg-6.0-essentials_build.zip", zip.c_str(), 0, NULL))) {
            // 解壓縮
            std::wstring cmd = L"powershell -Command \"Expand-Archive -Path '" + zip + L"' -DestinationPath '" + tempDir + L"' -Force\"";
            _wsystem(cmd.c_str());

            try {
                // 遞迴搜尋 bin 裡面的 exe 並搬移到當前目錄
                for (auto& p : fs::recursive_directory_iterator(tempDir)) {
                    if (p.path().filename() == "ffmpeg.exe") fs::rename(p.path(), "ffmpeg.exe");
                    else if (p.path().filename() == "ffprobe.exe") fs::rename(p.path(), "ffprobe.exe");
                }
                // 清理所有暫存
                if (fs::exists(zip)) fs::remove(zip);
                if (fs::exists(tempDir)) fs::remove_all(tempDir);
            } catch (...) {}
        }
    }
    PostMessage(hMainWnd, WM_INIT_DONE, 0, 0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        BOOL dark = TRUE; DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));
        HFONT hFont = CreateFontW(18, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft JhengHei UI");

        CreateWindowW(L"STATIC", L"網址輸入:", WS_VISIBLE|WS_CHILD, 15, 20, 75, 20, hwnd, NULL, NULL, NULL);
        hUrlInput = CreateWindowW(L"EDIT", L"", WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL, 95, 18, 355, 25, hwnd, NULL, NULL, NULL);
        CreateWindowW(L"BUTTON", L"增加項目", WS_VISIBLE|WS_CHILD, 460, 16, 95, 28, hwnd, (HMENU)ID_BTN_ADD, NULL, NULL);

        hLVUrls = CreateWindowExW(0, WC_LISTVIEWW, L"", WS_VISIBLE|WS_CHILD|WS_BORDER|LVS_REPORT|LVS_NOCOLUMNHEADER, 15, 55, 540, 120, hwnd, (HMENU)ID_LV_URLS, NULL, NULL);
        ListView_SetExtendedListViewStyle(hLVUrls, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
        ListView_SetBkColor(hLVUrls, g_bgCol); ListView_SetTextBkColor(hLVUrls, g_bgCol); ListView_SetTextColor(hLVUrls, g_txtCol);
        LVCOLUMNW lvc = {0}; lvc.mask = LVCF_WIDTH; lvc.cx = 510; ListView_InsertColumn(hLVUrls, 0, &lvc);
        HIMAGELIST hImg = ImageList_Create(1, 28, ILC_COLOR, 1, 1); ListView_SetImageList(hLVUrls, hImg, LVSIL_SMALL);

        CreateWindowW(L"STATIC", L"存放位置:", WS_VISIBLE|WS_CHILD, 15, 188, 75, 20, hwnd, NULL, NULL, NULL);
        wchar_t defPath[MAX_PATH]; GetCurrentDirectoryW(MAX_PATH, defPath); wcscat_s(defPath, L"\\Downloads");
        hPathInput = CreateWindowW(L"EDIT", defPath, WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL, 95, 185, 460, 25, hwnd, NULL, NULL, NULL);

        hFormatCombo = CreateWindowW(L"COMBOBOX", L"", WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST, 15, 222, 70, 200, hwnd, NULL, NULL, NULL);
        const wchar_t* fmts[] = { L"mp4", L"mkv", L"mp3", L"mpa", L"webm" };
        for(auto f : fmts) SendMessageW(hFormatCombo, CB_ADDSTRING, 0, (LPARAM)f);
        SendMessageW(hFormatCombo, CB_SETCURSEL, 0, 0);

        hResCombo = CreateWindowW(L"COMBOBOX", L"", WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST, 95, 222, 80, 200, hwnd, NULL, NULL, NULL);
        const wchar_t* ress[] = { L"2160p", L"1440p", L"1080p", L"720p" };
        for(auto r : ress) SendMessageW(hResCombo, CB_ADDSTRING, 0, (LPARAM)r);
        SendMessageW(hResCombo, CB_SETCURSEL, 2, 0);

        hFPSCombo = CreateWindowW(L"COMBOBOX", L"", WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST, 185, 222, 60, 200, hwnd, NULL, NULL, NULL);
        const wchar_t* fps[] = { L"60", L"50", L"40", L"30", L"20" };
        for(auto f : fps) SendMessageW(hFPSCombo, CB_ADDSTRING, 0, (LPARAM)f);
        SendMessageW(hFPSCombo, CB_SETCURSEL, 0, 0);

        hPlaylistCheck = CreateWindowW(L"BUTTON", L"播放清單模式", WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX, 260, 225, 130, 20, hwnd, NULL, NULL, NULL);
        SendMessageW(hPlaylistCheck, BM_SETCHECK, 1, 0);

        CreateWindowW(L"BUTTON", L"開始下載", WS_VISIBLE|WS_CHILD, 420, 220, 135, 30, hwnd, (HMENU)ID_BTN_START, NULL, NULL);
        hLogBox = CreateWindowW(L"EDIT", L"", WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL|ES_MULTILINE|ES_READONLY, 15, 260, 540, 100, hwnd, NULL, NULL, NULL);
        
        EnumChildWindows(hwnd, [](HWND c, LPARAM f) { SendMessage(c, WM_SETFONT, f, 1); return 1; }, (LPARAM)hFont);
        break;
    }
    case WM_INIT_DONE: { if (hSplash) { DestroyWindow(hSplash); hSplash = NULL; } ShowWindow(hwnd, SW_SHOW); return 0; }
    case WM_ERASEBKGND: { RECT rc; GetClientRect(hwnd, &rc); FillRect((HDC)wp, &rc, g_hbrBlack); return 1; }
    case WM_CTLCOLORSTATIC: { HDC hdc = (HDC)wp; SetTextColor(hdc, g_txtCol); SetBkColor(hdc, g_bgCol); return (LRESULT)g_hbrBlack; }
    case WM_CTLCOLOREDIT: { HDC hdc = (HDC)wp; SetTextColor(hdc, g_txtCol); SetBkColor(hdc, RGB(50, 50, 50)); return (LRESULT)g_hbrEdit; }
    case WM_COMMAND:
        if (LOWORD(wp) == ID_BTN_ADD) {
            wchar_t url[1024]; GetWindowTextW(hUrlInput, url, 1024);
            if (wcslen(url) > 0) {
                LVITEMW lvi = {0}; lvi.mask = LVIF_TEXT; lvi.iItem = ListView_GetItemCount(hLVUrls);
                lvi.pszText = (LPWSTR)url;
                int idx = ListView_InsertItem(hLVUrls, &lvi);
                ListView_SetCheckState(hLVUrls, idx, TRUE);
                SetWindowTextW(hUrlInput, L"");
            }
        }
        if (LOWORD(wp) == ID_BTN_START) std::thread(DownloadProcess).detach();
        break;
    case WM_DESTROY: PostQuitMessage(0); break;
    default: return DefWindowProcW(hwnd, msg, wp, lp);
    }
    return 0;
}

int main() {
    InitCommonControls();
    HINSTANCE hInst = GetModuleHandle(0);
    HICON hIcon = LoadIconW(hInst, MAKEINTRESOURCEW(1));
    if (!hIcon) hIcon = LoadIcon(NULL, IDI_APPLICATION);

    WNDCLASSEXW wc = {0}; 
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc; 
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
    wc.hbrBackground = g_hbrBlack; 
    wc.lpszClassName = L"JEX_PRO_FINAL_V2";
    wc.hIcon = hIcon; 
    wc.hIconSm = hIcon;

    if (!RegisterClassExW(&wc)) return 0;

    hMainWnd = CreateWindowExW(0, L"JEX_PRO_FINAL_V2", L"YT 播放清單抓取器 Pro", WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, (GetSystemMetrics(0)-585)/2, (GetSystemMetrics(1)-415)/2, 585, 415, NULL, NULL, hInst, NULL);
    hSplash = CreateWindowExW(WS_EX_TOPMOST, L"STATIC", L"\n\nSystem Initializing...", WS_POPUP|WS_VISIBLE|SS_CENTER|WS_BORDER, (GetSystemMetrics(0)-300)/2, (GetSystemMetrics(1)-100)/2, 300, 100, NULL, NULL, NULL, NULL);
    
    std::thread(InitializationThread).detach();

    MSG msg; 
    while (GetMessageW(&msg, 0, 0, 0)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    return 0;
}