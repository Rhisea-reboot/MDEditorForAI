# AGENTS.md

## 项目概述

这是一个基于Qt的Markdown编辑器GUI应用程序（MD Editor For AI - GUI），用于生成AI代理的Markdown文档。该工具允许用户通过图形界面扫描项目目录，提取指定扩展名的源代码文件，并生成格式化的Markdown文档供AI分析。

## 技术栈

- **框架**: Qt 5/6 (Widgets)
- **语言**: C++17
- **构建系统**: qmake (.pro文件)
- **目标文件名**: MDEditorGUI

## 项目结构

```
MarkdownEditor/
├── main.cpp              # 应用程序入口
├── MainWindow.h/.cpp     # 主窗口实现
├── mainwindow.ui         # UI布局文件（可选）
├── MarkdownEditor.pro    # Qt项目文件
├── MarkdownEditor_zh_CN.ts  # 中文翻译文件
├── core/                 # 核心功能模块
│   ├── getFile.h/.cpp    # 文件扫描和获取
│   └── initPrint.h/.cpp  # 初始化输出和格式化
└── build/                # 构建输出目录
```

## 核心功能模块

### getFile模块
- 扫描指定目录中的文件
- 支持扩展名过滤
- **支持 gitignore 风格的忽略列表**：
  - 精确匹配文件夹/文件名（如 `build`, `.git`, `node_modules`）
  - 通配符匹配（如 `*.log`, `temp*`, `*.tmp`）
  - 支持 `*`（任意字符）和 `?`（单个字符）模式
  - 路径中任何部分匹配都会被忽略
- 递归遍历子目录

### initPrint模块
- Markdown格式化输出
- 文件内容组织和结构化
- 生成AI可读的文档格式
- **代码压缩功能**（结构感知）：
  - **完整输出 (Full)**：保留原始代码
  - **骨架模式 (Skeleton)**：保留函数/类签名、#include、using/namespace、文档注释，函数体替换为 `{};` 或省略
  - **仅接口 (Interface Only)**：只保留声明（签名以 `;` 结尾），删除所有实现
  - 可选的跨文件 `#include` 去重
  - **支持语言**（17+ 种）：
    - **花括号类**：C/C++、Java、JavaScript/TypeScript/JSX/TSX、Go、Rust、C#、Swift、Kotlin、Scala、R、Julia、Lua
    - **缩进类**：Python
    - **end 关键字类**：Ruby、PHP

### MainWindow模块
- 项目配置输入（名称、路径、扩展名）
- 需求描述输入
- 忽略路径设置
- **代码压缩设置**：
  - 压缩模式下拉框（完整/骨架/仅接口）
  - `#include` 去重复选框
- 日志状态显示
- 设置持久化（QSettings）

## 构建说明

### 使用Qt Creator
1. 打开 `MarkdownEditor.pro`
2. 选择构建套件（Kits）
3. 点击构建/运行

### 使用命令行
```bash
# 生成Makefile
qmake MarkdownEditor.pro

# 编译
make        # Linux/Mac
nmake       # Windows (MSVC)
mingw32-make # Windows (MinGW)
```

## 代码规范

### 命名约定
- **类名**: PascalCase (MainWindow, GetFile)
- **函数名**: camelCase (onBrowseClicked, executeGeneration)
- **成员变量**: 以类型前缀 + 描述命名
  - QLineEdit *nameEdit
  - QPushButton *browseBtn
  - QTextEdit *logConsole
- **私有成员**: 无特殊前缀，通过上下文区分

### 文件组织
- 头文件（.h）放在项目根目录或core目录
- 实现文件（.cpp）与头文件同目录
- 第三方/核心模块放在core/子目录

### Qt特定规范
- 使用信号/槽机制进行组件通信
- 使用Q_OBJECT宏声明所有QObject派生类
- 使用QString进行字符串操作
- 使用QSettings保存应用配置
- 资源清理由QObject父子关系自动处理

## UI设计原则

- 使用QGroupBox组织相关功能区域
- 使用QFormLayout进行标签-输入框布局
- 使用QVBoxLayout作为顶层布局
- 日志区域使用深色背景（#1e1e1e）+ 等宽字体（Consolas）
- 操作按钮使用醒目的颜色标识（如生成按钮使用绿色 #4CAF50）

## 扩展指南

### 添加新的文件过滤器
编辑 `getFile.cpp` 中的过滤逻辑，支持新的文件扩展名或模式。

### 修改输出格式
编辑 `initPrint.cpp` 中的格式化函数，调整Markdown输出结构。

### 添加新的UI组件
1. 在 `MainWindow.h` 中声明新组件
2. 在 `MainWindow.cpp` 构造函数中初始化和布局
3. 连接必要的信号/槽

### 添加新的语言压缩支持
编辑 `initPrint.cpp`：
1. 在 `shouldCompressFile()` 中添加新的扩展名
2. 在 `compressCode()` 的 dispatch 逻辑中添加新分支
3. 根据语言语法选择压缩策略：
   - **花括号类**（C-family）：复用 `compressCppSkeleton()` / `compressCppInterface()`
   - **缩进类**（如 Python）：复用 `compressPySkeleton()` 或实现缩进分析
   - **end 关键字类**（如 Ruby）：复用 `compressRubyLikeSkeleton()` 或实现 end 计数器
4. 如需特殊处理，可新增 `compressXxxSkeleton()` 函数

## 调试提示

- 日志区域显示彩色状态信息（黑色、青色等）
- 使用 `log()` 函数向日志区域添加信息
- 设置通过QSettings自动保存和恢复

## 依赖项

- QtCore
- QtGui
- QtWidgets
- C++17标准库
