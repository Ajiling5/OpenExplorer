#define _WIN32_WINNT 0x0600
#define NTDDI_VERSION 0x06000000
#define UNICODE
#define _UNICODE

#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <stdio.h>
#include <wchar.h>

#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "shell32.lib")

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR pCmdLine, int nCmdShow)
{
    OPENFILENAMEW ofn;
    WCHAR szFile[MAX_PATH] = {0};

    /* 清空结构体 */
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = NULL;
    ofn.lpstrFile   = szFile;
    ofn.nMaxFile    = MAX_PATH;
    ofn.lpstrFilter = L"All Files\0*.*\0"
                      L"Executables\0*.exe\0"
                      L"Text Files\0*.txt\0"
                      L"Batch Files\0*.bat\0"
                      L"\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle   = L"Open Explorer (PE Edition)";
    ofn.Flags = OFN_PATHMUSTEXIST
              | OFN_FILEMUSTEXIST
              | OFN_EXPLORER
              | OFN_HIDEREADONLY;

    /* ===== 循环：选完文件打开后，不关闭，继续弹 ===== */
    while (1) {
        /* 每次循环重新清空文件名缓冲区 */
        szFile[0] = L'\0';

        BOOL ok = GetOpenFileNameW(&ofn);

        if (!ok) {
            /* 用户取消或出错，退出循环 */
            DWORD err = CommDlgExtendedError();
            if (err != 0) {
                /* 有扩展错误码，可以在这里记录，但为了简单直接退出 */
            }
            break;
        }

        /* 用默认程序打开选中的文件 */
        HINSTANCE hInst = ShellExecuteW(
            NULL,
            L"open",
            szFile,
            NULL,
            NULL,
            SW_SHOWNORMAL
        );

        /* 如果 ShellExecute 失败，可以在这里弹个 MessageBoxW 提示 */
        if ((INT_PTR)hInst <= 32) {
            WCHAR szMsg[512];
            wsprintfW(szMsg, L"Failed to open:\n%s\nError code: %Id",
                      szFile, (INT_PTR)hInst);
            MessageBoxW(NULL, szMsg, L"Open Explorer", MB_ICONWARNING);
        }

        /* 循环回来，再次弹出对话框 */
    }

    return 0;
}
