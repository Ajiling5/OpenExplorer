# OpenExplorer

Open File,Open Source,OpenExplorer.

> A standalone Win32 file picker that works when Explorer doesn't.
> 一个不依赖 Explorer、DWM、Sihost 的独立 Win32 文件选择器。

---

## 📖 项目简介

`OpenExplorer` 是一个用 C 语言编写的轻量级文件选择器，基于 `GetOpenFileNameW`（`comdlg32.dll`）和 `IFileOpenDialog`（`shell32.dll`）两套 API，提供两种实现版本：

| 版本 | 源文件 | 适用环境 |
|---|---|---|
| **IFileOpenDialog 版** | `OpenExplorer.c` | 正常 Windows，支持完整 Shell 上下文菜单 |
| **GetOpenFileNameW 版（PE Edition）** | `OpenExplorerPE.c` | ADK PE、精简 PE |

在以下环境中，`OpenExplorer` 可以作为文件选择器使用：

- **ADK PE / WinPE** —— WinXShell 文件管理器打不开时
- **精简 PE** —— 没有 `explorer.exe`、`sihost.exe` 时
- **图形 Shell 崩溃的 Windows** —— `dwm.exe`、`sihost.exe` 被删除或损坏时
- **正常 Windows** —— Win11 24H2 运行框砍掉“浏览”按钮后，可作为替代方案

---

## ⚙️ 核心逻辑

项目使用最底层的 Win32 API 构建，**不依赖 COM 初始化、不依赖 Shell 命名空间、不依赖 DWM**。

```c
while (1) {
    BOOL ok = GetOpenFileNameW(&ofn);
    if (!ok) break;

    ShellExecuteW(NULL, L"open", szFile, NULL, NULL, SW_SHOWNORMAL);
}
