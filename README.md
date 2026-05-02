# MD Editor For AI - GUI

[![Qt Version](https://img.shields.io/badge/Qt-5%2F6-green.svg)](https://www.qt.io/)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/license-MIT-yellow.svg)](LICENSE)

一个基于 Qt 的 GUI 应用程序，用于生成 AI 代理可读的 Markdown 文档。通过扫描项目目录，提取源代码文件，并生成格式化的 Markdown 文档供 AI 分析。

![Application Preview](docs/preview.png)

## 功能特性

### 核心功能
- **项目扫描**：递归扫描指定目录，提取指定扩展名的源代码文件
- **Gitignore 风格忽略**：支持通配符和精确匹配的忽略模式（如 `build`, `.git`, `*.log`）
- **代码压缩**：
  - **完整输出 (Full)**：保留原始代码
  - **骨架模式 (Skeleton)**：保留签名和关键结构，省略函数体（默认）
  - **仅接口 (Interface Only)**：只保留声明，删除所有实现
- **#include 去重**：可选的跨文件重复 #include 去除
- **设置持久化**：自动保存和恢复用户配置

### 支持的文件类型
- **C/C++**：`.cpp`, `.h`, `.hpp`, `.c`, `.cc`, `.cxx`
- **Java**：`.java`
- **Python**：`.py`
- **JavaScript/TypeScript**：`.js`, `.ts`
- **Go**：`.go`
- **Rust**：`.rs`
- **C#**：`.cs`
- **其他**：`.swift`, `.kt`, `.scala`, `.rb`, `.php`

## 快速开始

### 环境要求
- Qt 5.15+ 或 Qt 6.x
- C++17 兼容的编译器（GCC, MSVC, Clang）
- qmake 或 CMake

### 构建步骤

#### 使用 Qt Creator
1. 打开 `MarkdownEditor.pro`
2. 选择构建套件（Kits）
3. 点击构建/运行

#### 使用命令行
```bash
# 生成 Makefile
qmake MarkdownEditor.pro

# 编译
make        # Linux/Mac
nmake       # Windows (MSVC)
mingw32-make # Windows (MinGW)
```

### 使用方法

1. **启动应用**：运行 `MDEditorGUI.exe`（Windows）或 `MDEditorGUI`（Linux/Mac）

2. **配置项目**：
   - **Project Name**：输入项目名称（用于生成 .md 文件名）
   - **Target Path**：选择要扫描的项目目录
   - **Extensions**：输入文件扩展名（空格分隔，如 `cpp h hpp`）
   - **Ignore List**：输入要忽略的文件/文件夹（空格分隔，支持通配符）
   - **Compression**：选择代码压缩级别

3. **输入需求**：在 Requirements 区域输入项目需求描述

4. **生成文档**：点击 **GENERATE MARKDOWN** 按钮

5. **查看输出**：生成的 Markdown 文件位于当前目录，命名为 `[ProjectName].md`

## 项目结构

```
MarkdownEditor/
├── main.cpp                  # 应用程序入口
├── MainWindow.h/.cpp        # 主窗口实现
├── mainwindow.ui            # UI 布局文件（Qt Designer）
├── MarkdownEditor.pro       # Qt 项目文件
├── MarkdownEditor_zh_CN.ts  # 中文翻译文件
├── AGENTS.md                # AI 代理文档（本项目文档）
├── README.md                # 本文件
├── core/                    # 核心功能模块
│   ├── getFile.h/.cpp       # 文件扫描和获取
│   └── initPrint.h/.cpp     # 初始化输出和代码压缩
└── build/                   # 构建输出目录
```

## 技术栈

- **框架**：Qt 5/6 (Widgets)
- **语言**：C++17
- **构建系统**：qmake
- **目标文件名**：MDEditorGUI

## 配置示例

### 示例 1：扫描 C++ 项目
```
Project Name: MyCppProject
Target Path: /path/to/my/project
Extensions: cpp h hpp
Ignore List: .git build .vscode *.tmp
Compression: Skeleton (骨架模式)
去重复 #include: [✓]
```

### 示例 2：扫描 Python 项目
```
Project Name: MyPythonProject
Target Path: /path/to/python/project
Extensions: py
Ignore List: __pycache__ .git venv *.pyc
Compression: Full (完整输出)
去重复 #include: [ ]
```

## 压缩模式说明

| 模式 | 保留内容 | 删除/替换 | 适用场景 |
|------|---------|----------|---------|
| **Full** | 全部 | - | 首次完整分析 |
| **Skeleton** | 函数/类签名、`#include`、`using/namespace`、Qt 宏 | 函数体 → `// ... 实现省略` | 日常迭代（推荐） |
| **Interface Only** | 类定义、函数声明 | 所有实现 | 架构确认 |

## 忽略模式语法

支持 Gitignore 风格的模式匹配：

| 模式类型 | 示例 | 说明 |
|---------|------|------|
| 精确匹配 | `.git`, `build`, `node_modules` | 匹配完整文件名或文件夹名 |
| 通配符 `*` | `*.log`, `temp*`, `*.tmp` | `*` 匹配任意字符 |
| 通配符 `?` | `file?.txt` | `?` 匹配单个字符 |

## 开发指南

### 添加新的文件过滤器
编辑 `core/getFile.cpp` 中的过滤逻辑，支持新的文件扩展名或模式。

### 修改输出格式
编辑 `core/initPrint.cpp` 中的格式化函数，调整 Markdown 输出结构。

### 添加新的 UI 组件
1. 在 `MainWindow.h` 中声明新组件
2. 在 `MainWindow.cpp` 构造函数中初始化和布局
3. 连接必要的信号/槽

## 常见问题

**Q: 生成的 Markdown 文件在哪里？**  
A: 默认在项目根目录，命名为 `[ProjectName].md`

**Q: 支持哪些 Qt 版本？**  
A: Qt 5.15+ 和 Qt 6.x 均可

**Q: 骨架模式会丢失重要代码吗？**  
A: 不会。骨架模式保留所有接口定义和关键结构，仅省略函数体实现

**Q: 如何恢复默认设置？**  
A: 删除系统配置文件（Windows: 注册表，Linux/Mac: QSettings 存储位置）

## 贡献

欢迎提交 Issue 和 Pull Request！

## 许可证

[MIT License](LICENSE)

## 致谢

- [Qt Framework](https://www.qt.io/) - 跨平台 GUI 框架
- C++17 标准库

---

**Made with ❤️ for AI-assisted development**
