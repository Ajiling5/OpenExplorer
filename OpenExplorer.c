/* ============================================================
 * Open Explorer
 * 适配 TDM-GCC 4.9.2 (64-bit) / MinGW-w64
 * 依赖：OLE32, UUID, SHELL32
 * ============================================================ */

#define _WIN32_WINNT 0x0600
#define NTDDI_VERSION 0x06000000
#define UNICODE
#define _UNICODE

#include <windows.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <stdio.h>
#include <wchar.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "shell32.lib")

int wmain(void)
{
    HRESULT hr;
    IFileOpenDialog *pFileOpen = NULL;
    IShellItem *pItem = NULL;
    PWSTR pszFilePath = NULL;

    /* 1. 初始化 COM */
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        wprintf(L"[!] CoInitializeEx failed: 0x%08X\n", (unsigned int)hr);
        return 1;
    }

    /* 2. 创建文件打开对话框 */
    hr = CoCreateInstance(
        &CLSID_FileOpenDialog,
        NULL,
        CLSCTX_INPROC_SERVER,
        &IID_IFileOpenDialog,
        (void **)&pFileOpen
    );

    if (FAILED(hr)) {
        wprintf(L"[!] CoCreateInstance failed: 0x%08X\n", (unsigned int)hr);
        CoUninitialize();
        return 1;
    }

    /* 3. 设置选项 */
    DWORD dwOptions = 0;
    pFileOpen->lpVtbl->GetOptions(pFileOpen, &dwOptions);
    pFileOpen->lpVtbl->SetOptions(
        pFileOpen,
        dwOptions | FOS_FILEMUSTEXIST | FOS_FORCEFILESYSTEM
    );

    pFileOpen->lpVtbl->SetTitle(pFileOpen, L"Open Explorer");
    pFileOpen->lpVtbl->SetFileName(pFileOpen, L"");

    /* 4. 显示对话框 */
    hr = pFileOpen->lpVtbl->Show(pFileOpen, NULL);

    if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        wprintf(L"[*] User cancelled.\n");
        pFileOpen->lpVtbl->Release(pFileOpen);
        CoUninitialize();
        return 0;
    }

    if (FAILED(hr)) {
        wprintf(L"[!] Show failed: 0x%08X\n", (unsigned int)hr);
        pFileOpen->lpVtbl->Release(pFileOpen);
        CoUninitialize();
        return 1;
    }

    /* 5. 获取选中项 */
    hr = pFileOpen->lpVtbl->GetResult(pFileOpen, &pItem);
    if (SUCCEEDED(hr) && pItem != NULL) {

        hr = pItem->lpVtbl->GetDisplayName(
            pItem,
            SIGDN_FILESYSPATH,
            &pszFilePath
        );

        if (SUCCEEDED(hr) && pszFilePath != NULL) {
            wprintf(L"[+] Selected: %s\n", pszFilePath);

            /* 6. 用默认程序打开文件 */
            HINSTANCE hInst = ShellExecuteW(
                NULL,
                L"open",
                pszFilePath,
                NULL,
                NULL,
                SW_SHOWNORMAL
            );

            if ((INT_PTR)hInst <= 32) {
                wprintf(L"[!] ShellExecute failed: %Id\n", (INT_PTR)hInst);
            } else {
                wprintf(L"[+] Opened successfully.\n");
            }

            CoTaskMemFree(pszFilePath);
        } else {
            wprintf(L"[!] GetDisplayName failed: 0x%08X\n", (unsigned int)hr);
        }

        pItem->lpVtbl->Release(pItem);
    } else {
        wprintf(L"[!] GetResult failed: 0x%08X\n", (unsigned int)hr);
    }

    pFileOpen->lpVtbl->Release(pFileOpen);
    CoUninitialize();
    return 0;
}
