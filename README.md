# MD Editor for AI

一个专为AI助手设计的Markdown文档生成工具，能够自动化生成包含项目结构、代码实现和需求的完整Markdown文档，特别适合与ChatGPT等AI编程助手配合使用。

## 🌟 功能特性

- **可视化界面**：基于Qt的GUI界面，操作简单直观
- **智能项目分析**：自动扫描项目目录结构并生成树状图
- **代码提取**：支持多种编程语言（C++、Python、Java等）的源代码提取
- **模板化输出**：按照标准格式生成Markdown文档
- **配置灵活**：可自定义需要提取的文件扩展名
- **实时日志**：详细的操作日志和状态反馈

## 🏗️ 项目结构

```
MDEditorForAI/
├── core/                 # 核心功能模块
│   ├── getFile.cpp      # 目录遍历和代码提取
│   ├── getFile.h
│   ├── initPrint.cpp    # Markdown初始化
│   └── initPrint.h
├── main.cpp             # 程序入口
├── MainWindow.cpp       # GUI主窗口实现
├── MainWindow.h
├── mainwindow.ui        # UI设计文件
├── MarkdownEditor.pro   # Qt项目配置文件
└── README.md           # 项目说明文档
```

## 📋 系统要求

- **操作系统**：Windows、Linux或macOS
- **编译器**：支持C++17的编译器（GCC 7+、Clang 5+、MSVC 2017+）
- **Qt版本**：Qt 5.15或更高版本
- **构建系统**：qmake或CMake

## 🚀 快速开始

### 1. 克隆仓库
```bash
git clone https://github.com/yourusername/MDEditorForAI.git
cd MDEditorForAI
```

### 2. 构建项目
#### 使用Qt Creator：
- 打开`MarkdownEditor.pro`文件
- 选择构建套件并构建项目

#### 使用命令行：
```bash
qmake MarkdownEditor.pro
make  # 或在Windows上：nmake 或 mingw32-make
```

### 3. 运行程序
```bash
./MDEditorGUI  # Linux/macOS
MDEditorGUI.exe  # Windows
```

## 📖 使用方法

### 基本流程：
1. **配置项目**
   - 输入项目名称
   - 选择项目根目录
   - 设置需要提取的文件扩展名（如：`cpp h hpp py java`）

2. **输入需求**
   - 在需求文本框中详细描述项目需求

3. **生成文档**
   - 点击"GENERATE MARKDOWN"按钮
   - 程序将自动生成包含以下内容的Markdown文件：
     - 项目目录结构
     - 需求描述
     - 完整的源代码实现

### 生成文档示例：
```markdown
# 身份
你是一个编程高手...

# 项目规则
...

# 项目需求
[用户输入的需求]

# 项目目录结构
├── src/
│   ├── main.cpp
│   └── utils.h

# 项目代码实现
main.cpp
```cpp
#include <iostream>
...
```

utils.h
```h
#ifndef UTILS_H
...
```

## ⚙️ 配置选项

### 支持的文件扩展名
默认支持：`.cpp`、`.h`、`.hpp`

可通过界面配置支持任意扩展名，例如：
- Python项目：`.py`
- Java项目：`.java`
- Web项目：`.js` `.html` `.css`

### 自定义规则
在项目根目录创建`rules.md`文件，程序会自动将其内容插入到生成的Markdown文档中。

## 🔧 技术实现

### 核心模块
- **getFile模块**：负责文件系统遍历和代码提取
- **initPrint模块**：负责Markdown文档的初始化和格式化
- **MainWindow模块**：提供图形用户界面

### 关键技术
- 使用C++17标准库（特别是filesystem）
- 基于Qt框架构建跨平台GUI
- 递归目录遍历算法
- Markdown语法自动生成

## 📝 开发说明

### 代码规范
- 使用驼峰命名法
- 遵循RAII原则管理资源
- 异常安全设计
- 详细的注释文档

### 扩展开发
要添加新功能，请遵循以下原则：
1. 保持向后兼容性
2. 新增函数使用驼峰命名法
3. 修改已有代码时添加详细注释
4. 确保跨平台兼容性

## 🤝 贡献指南

欢迎提交Issue和Pull Request！

### 贡献流程：
1. Fork本仓库
2. 创建功能分支
3. 提交更改
4. 推送到分支
5. 创建Pull Request

### 代码要求：
- 通过代码审查
- 添加适当的测试
- 更新相关文档

## 📄 许可证

本项目采用MIT许可证。详见[LICENSE](LICENSE)文件。

## 📞 支持与反馈

- **问题反馈**：请使用GitHub Issues
- **功能建议**：欢迎提出新想法
- **贡献代码**：Pull Request随时开放

## 🙏 致谢

感谢所有为这个项目做出贡献的开发者！

---

**温馨提示**：生成的Markdown文档可以直接复制到ChatGPT等AI助手的对话中，帮助AI更好地理解你的项目结构和代码实现。

---


