
```markdown
# OpenExplorer

Open File, Open Source, OpenExplorer.

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
- **正常 Windows** —— Win11 运行框砍掉“浏览”按钮后，可作为替代方案

---

## ⚙️ 核心逻辑

项目使用最底层的 Win32 API 构建，**不依赖 COM 初始化、不依赖 Shell 命名空间、不依赖 DWM**。

```c
while (1) {
    BOOL ok = GetOpenFileNameW(&ofn);
    if (!ok) break;

    ShellExecuteW(NULL, L"open", szFile, NULL, NULL, SW_SHOWNORMAL);
}
```

- 选完文件后**不关闭对话框**，继续弹出，支持连续打开多个文件
- 点“取消”或关闭窗口时退出循环

---

## ✨ 功能

| 功能 | 状态 | 说明 |
|---|---|---|
| 浏览文件系统 | ✅ | 支持所有盘符，包括 PE 的 `X:\` |
| 选择文件 | ✅ | 单选 |
| 打开文件 | ✅ | 调 `ShellExecuteW`，用默认程序打开 |
| 拖出文件 | ✅ | OLE 拖放 |
| 拖入文件 | ✅ | OLE 拖放 |
| 右键菜单 | ✅ | `IContextMenu` |
| 复制 / 剪切 / 粘贴 | ✅ | `IFileOperation` |
| 删除 / 重命名 | ✅ | `IFileOperation` |
| 创建快捷方式 | ✅ | `IShellLink` |
| 属性 / 高级属性 | ✅ | `IShellItem2` |
| 查看模式切换 | ✅ | `IFolderView2` |
| 多选文件 | ❌ | 可加 `OFN_ALLOWMULTISELECT` 支持 |
| 预览窗格 | ❌ | 需要 `IThumbnailProvider` |

> 右键菜单、复制、删除、重命名、属性、创建快捷方式、查看模式切换等功能，由 `shell32.dll` 提供，`OpenExplorer` 只是成功调用了它们。

---

## 🛠️ 编译

### 环境要求

- Windows 7 及以上（推荐 Windows 10 / 11）
- MinGW-w64 或 TDM-GCC（Dev-C++ 自带）
- 或 Visual Studio（MSVC）

### 依赖库

| 库 | IFileOpenDialog 版 | GetOpenFileNameW 版（PE Edition） |
|---|---|---|
| `comdlg32` | ❌ | ✅ |
| `shell32` | ✅ | ✅ |
| `ole32` | ✅ | ❌ |
| `uuid` | ✅ | ❌ |

### 命令行编译（MinGW-w64 / TDM-GCC）

**IFileOpenDialog 版：**

```bash
gcc OpenExplorer.c -o OpenExplorer.exe -mwindows -municode -lole32 -luuid -lshell32
```

**GetOpenFileNameW 版（PE Edition）：**

```bash
gcc OpenExplorerPE.c -o OpenExplorerPE.exe -mwindows -municode -lcomdlg32 -lshell32
```

### Dev-C++ 编译配置

**1. 项目属性 → 参数 → 编译器：**

```
-municode
```

**2. 项目属性 → 参数 → 链接器：**

IFileOpenDialog 版：

```
-mwindows -municode -lole32 -luuid -lshell32
```

GetOpenFileNameW 版（PE Edition）：

```
-mwindows -municode -lcomdlg32 -lshell32
```

**3. 如果编译时报 `undefined reference to 'WinMain'`：**

- 检查是否加了 `-municode`
- 检查入口函数是否为 `wWinMain`（Unicode 版）
- 如果用 `WinMain`，则**不要**加 `-municode`

**4. 如果编译时报 `undefined reference to __imp_CoTaskMemFree`：**

- 检查是否加了 `-lole32 -luuid`
- 检查是否加了 `#pragma comment(lib, "ole32.lib")`

**5. 如果编译时报 `undefined reference to 'CLSID_FileOpenDialog'`：**

- 在源文件开头加：

```c
#define _WIN32_WINNT 0x0600
#define NTDDI_VERSION 0x06000000
```

- 并确保链接了 `-luuid`

### Visual Studio 编译

**1. 创建空项目，添加源文件**

**2. 项目属性 → 链接器 → 输入 → 附加依赖项：**

IFileOpenDialog 版：

```
ole32.lib;uuid.lib;shell32.lib
```

GetOpenFileNameW 版（PE Edition）：

```
comdlg32.lib;shell32.lib
```

**3. 项目属性 → C/C++ → 预处理器 → 预处理器定义：**

```
UNICODE;_UNICODE;_WIN32_WINNT=0x0600;NTDDI_VERSION=0x06000000
```

**4. 项目属性 → 链接器 → 系统 → 子系统：**

```
Windows (/SUBSYSTEM:WINDOWS)
```

**5. 入口点：** Visual Studio 默认会自动识别 `wWinMain`，无需手动指定。

### 常见错误速查表

| 错误 | 原因 | 解决方法 |
|---|---|---|
| `undefined reference to 'WinMain'` | 入口点与 `-municode` 不匹配 | 用 `wWinMain` + `-municode`，或用 `WinMain` 不加 `-municode` |
| `undefined reference to __imp_CoTaskMemFree` | 没链接 `ole32` | 加 `-lole32` |
| `undefined reference to 'CLSID_FileOpenDialog'` | 没链接 `uuid` | 加 `-luuid`，并加 `NTDDI_VERSION` 宏 |
| `undefined reference to 'IID_IFileOpenDialog'` | 同上 | 同上 |
| `undefined reference to 'ShellExecuteW'` | 没链接 `shell32` | 加 `-lshell32` |
| `undefined reference to 'GetOpenFileNameW'` | 没链接 `comdlg32` | 加 `-lcomdlg32` |
| `converting to execution character set: Illegal byte sequence` | 源码里有中文字符串 | 改成英文，或加 `-finput-charset=UTF-8 -fexec-charset=UTF-8` |
| `'IFileOpenDialog' undeclared` | 缺 `NTDDI_VERSION` 宏 | 在源文件开头加 `#define NTDDI_VERSION 0x06000000` |

### 编译产物

| 源文件 | 输出 | 大小 |
|---|---|---|
| `OpenExplorer.c` | `OpenExplorer.exe` | ~135 KB |
| `OpenExplorerPE.c` | `OpenExplorerPE.exe` | ~135 KB |

**单文件、无外部依赖、不需要安装运行库。** 直接拷进 PE 或正常 Windows 即可运行。

---

## 🚀 使用

1. 编译得到 `OpenExplorer.exe` 或 `OpenExplorerPE.exe`
2. 在 PE 或正常 Windows 里双击运行
3. 弹出文件选择框，选中文件，点“打开”
4. 文件用默认程序打开，对话框**继续弹出**
5. 点“取消”或关闭窗口退出

### 全局调用（可选）

把 exe 拷进 `D:\PATH\`，把 `D:\PATH\` 加入系统 `Path` 环境变量。之后：

```
Win+R → 输入 OpenEx → 弹出文件选择器
cmd → 输入 OpenEx → 弹出文件选择器
```

---

## 🧪 已验证环境

| 环境 | IFileOpenDialog 版 | GetOpenFileNameW 版 |
|---|---|---|
| **正常 Windows 11** | ✅ | ✅ |
| **ADK PE** | ⚠️ 依赖 Shell 命名空间 | ✅ |
| **精简 PE** | ❌ | ✅ |
| **脱衣 Win11（DWM/Sihost 被删）** | ❌ | ⚠️ 取决于 Shell 残留 |

### 在 ADK PE 里已验证的功能

- 浏览 `X:\Windows`、`C:\` 等盘符
- 拖出 / 拖入文件
- 右键菜单
- 复制、重命名、删除
- 创建快捷方式
- 属性 / 高级属性
- 查看模式切换
- 跨虚拟机拖放（VMware）

---

## 📁 项目结构

```
OpenExplorer/
├── README.md
├── README_AI.md          # 100% AI 生成的幻觉版（彩蛋）
├── LICENSE
├── .gitignore
├── src/
│   ├── OpenExplorer.c    # IFileOpenDialog 版
│   └── OpenExplorerPE.c  # GetOpenFileNameW 版（PE Edition）
└── docs/
    └── screenshots/      # PE 环境下的运行截图
```

---

## 📜 背景

这个项目诞生于一次“Windows 11 脱衣实验”：

1. 用 `rename` 把 `dwm.exe`、`sihost.exe`、`Resources`、`SystemApps` 改成 `.bak`
2. 用 `cmd.exe` 伪装成 `explorer.exe`
3. 系统退化成 `WINDOS`：黑屏、TUI 登录、命令行 Shell
4. 在 ADK PE 里发现 WinXShell 的文件管理器打不开
5. 用记事本的打开功能当文件管理器，调侃为Open Explorer
6. 于是写了 `OpenExplorer` 来补上这个缺口

> "The Window is gone. The DOS remains."

---

## 📄 许可证

MIT License

---

## 🙏 致谢

- `comdlg32.dll`、`shell32.dll` —— 提供了底层 API
- ADK PE —— 提供了测试环境
- 所有在 `WINDOS` 里幸存的 Win32 老兵
