#include "MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QDateTime>
#include <QApplication>
#include <QScrollBar>
#include <QFileInfo>
// [FIX END]
#include <fstream>
#include <vector>
#include <sstream>

// 引入原项目的核心头文件
#include "core/getFile.h"
#include "core/initPrint.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    setWindowTitle("MD Editor For AI - GUI Version");
    resize(600, 750);

    // --- UI 布局构建 ---
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 1. 项目配置区域
    QGroupBox *configGroup = new QGroupBox("Project Configuration", this);
    QFormLayout *formLayout = new QFormLayout(configGroup);

    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("e.g., MyNewFeature");
    formLayout->addRow("Project Name:", nameEdit);

    QHBoxLayout *pathLayout = new QHBoxLayout();
    pathEdit = new QLineEdit(this);
    pathEdit->setPlaceholderText("Select the target project root directory");
    browseBtn = new QPushButton("Browse...", this);
    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(browseBtn);
    formLayout->addRow("Target Path:", pathLayout);

    extEdit = new QLineEdit(this);
    extEdit->setText("cpp h hpp"); // 默认值
    extEdit->setPlaceholderText("Space separated extensions (e.g., py java cpp)");
    formLayout->addRow("Extensions:", extEdit);

    QHBoxLayout *ignoreLayout = new QHBoxLayout();
    ignoreEdit = new QLineEdit(this);
    ignoreEdit->setPlaceholderText("Space separated (e.g., .git .vscode build node_modules)");
    ignoreEdit->setToolTip("Folders or files to exclude from the scan");

    browseIgnoreBtn = new QPushButton("Browse",this);

    ignoreLayout->addWidget(ignoreEdit);
    ignoreLayout->addWidget(browseIgnoreBtn);
    formLayout->addRow("Ignore List:", ignoreLayout);

    configGroup->setLayout(formLayout);
    mainLayout->addWidget(configGroup);

    // 2. 需求输入区域
    QGroupBox *reqGroup = new QGroupBox("Requirements", this);
    QVBoxLayout *reqLayout = new QVBoxLayout(reqGroup);
    reqEdit = new QTextEdit(this);
    reqEdit->setPlaceholderText("Enter detailed project requirements here...");
    reqLayout->addWidget(reqEdit);
    reqGroup->setLayout(reqLayout);
    mainLayout->addWidget(reqGroup);

    // 3. 操作区域
    generateBtn = new QPushButton("GENERATE MARKDOWN", this);
    generateBtn->setMinimumHeight(40);
    generateBtn->setStyleSheet("font-weight: bold; background-color: #4CAF50; color: white;");
    mainLayout->addWidget(generateBtn);

    // 4. 日志区域
    QGroupBox *logGroup = new QGroupBox("Status Log", this);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logConsole = new QTextEdit(this);
    logConsole->setReadOnly(true);
    logConsole->setStyleSheet("background-color: #1e1e1e; color: #d4d4d4; font-family: Consolas;");
    logLayout->addWidget(logConsole);
    logGroup->setLayout(logLayout);
    mainLayout->addWidget(logGroup);

    // --- 信号槽连接 ---
    connect(browseBtn, &QPushButton::clicked, this, &MainWindow::onBrowseClicked);
    connect(generateBtn, &QPushButton::clicked, this, &MainWindow::onGenerateClicked);
    connect(browseIgnoreBtn, &QPushButton::clicked, this, &MainWindow::onBrowseIgnoreClicked);

    // 初始化日志
    log("System Ready. Waiting for input...", "cyan");
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::saveSettings()
{
    QSettings settings("MyCompany", "MDEditorForAI");
    settings.setValue("projectName", nameEdit->text());
    settings.setValue("targetPath", pathEdit->text());
    settings.setValue("extensions", extEdit->text());
    settings.setValue("ignoreList", ignoreEdit->text());
}

void MainWindow::loadSettings()
{
    QSettings settings("MyCompany", "MDEditorForAI");
    nameEdit->setText(settings.value("projectName", "").toString());
    pathEdit->setText(settings.value("targetPath", "").toString());
    extEdit->setText(settings.value("extensions", "cpp h hpp").toString()); // 默认值
    ignoreEdit->setText(settings.value("ignoreList", ".git .vscode build .idea bin obj").toString()); // 默认忽略
}

void MainWindow::log(const QString &msg, const QString &color)
{
    QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss] ");
    QString html = QString("<span style='color: gray;'>%1</span><span style='color: %2;'>%3</span>")
                       .arg(timestamp, color, msg);
    logConsole->append(html);
    QScrollBar *sb = logConsole->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MainWindow::onBrowseClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Project Directory",
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        pathEdit->setText(dir);
        log("Directory selected: " + dir, "green");
    }
}

void MainWindow::onBrowseIgnoreClicked()
{
    // 如果已选择了 Target Path，则从该路径开始，否则从 Home 开始
    QString startPath = pathEdit->text().isEmpty() ? QDir::homePath() : pathEdit->text();

    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory to Ignore",
                                                    startPath,
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        QFileInfo info(dir);
        QString dirName = info.fileName(); // 仅获取文件夹名称（如 build），而不是完整路径

        QString currentText = ignoreEdit->text().trimmed();
        if (!currentText.isEmpty()) {
            currentText += " "; // 添加空格分隔
        }
        currentText += dirName;

        ignoreEdit->setText(currentText);
        log("Added to ignore list: " + dirName, "green");
    }
}

void MainWindow::onGenerateClicked()
{
    // 基础校验
    QString name = nameEdit->text().trimmed();
    QString path = pathEdit->text().trimmed();
    QString reqs = reqEdit->toPlainText();
    QString exts = extEdit->text().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Project Name cannot be empty.");
        return;
    }
    if (path.isEmpty() || !std::filesystem::exists(path.toStdString())) {
        QMessageBox::warning(this, "Input Error", "Invalid Directory Path.");
        return;
    }

    // 禁用按钮防止重复点击
    generateBtn->setEnabled(false);
    generateBtn->setText("Processing...");
    QApplication::processEvents(); // 刷新 UI

    try {
        executeGeneration();
        QMessageBox::information(this, "Success", "Markdown file generated successfully!");
    } catch (const std::exception &e) {
        log("CRITICAL ERROR: " + QString(e.what()), "red");
        QMessageBox::critical(this, "Error", QString("An error occurred:\n%1").arg(e.what()));
    } catch (...) {
        log("UNKNOWN ERROR OCCURRED", "red");
    }

    generateBtn->setEnabled(true);
    generateBtn->setText("GENERATE MARKDOWN");
}

void MainWindow::executeGeneration()
{
    std::string nameStr = nameEdit->text().trimmed().toStdString();
    std::string pathStr = pathEdit->text().trimmed().toStdString();
    std::string outputFileName = nameStr + ".md";

    // 准备后缀列表
    std::vector<std::string> targetExtensions;
    QStringList extList = extEdit->text().split(' ', Qt::SkipEmptyParts);
    if (extList.isEmpty()) {
        targetExtensions = {".cpp", ".h", ".hpp"};
        log("Using default extensions: .cpp, .h, .hpp", "yellow");
    } else {
        QString logMsg = "Target extensions: ";
        for (const QString &ext : extList) {
            QString cleanExt = ext;
            if (!cleanExt.startsWith('.')) cleanExt.prepend('.');
            targetExtensions.push_back(cleanExt.toStdString());
            logMsg += cleanExt + " ";
        }
        log(logMsg, "yellow");
    }

    std::vector<std::string> ignoreList;
    QStringList ignoreStrList = ignoreEdit->text().split(' ', Qt::SkipEmptyParts);
    if (!ignoreStrList.isEmpty()) {
        QString logMsg = "Ignoring: ";
        for (const QString &item : ignoreStrList) {
            ignoreList.push_back(item.toStdString());
            logMsg += item + " ";
        }
        log(logMsg, "yellow");
    }

    char buffer[256];
    // 安全复制文件名到 buffer
    strncpy(buffer, outputFileName.c_str(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    // --- STEP 1: 初始化文件 (调用 initPrint) ---
    log("Step 1: Initializing markdown file...", "cyan");
    if (!std::filesystem::exists("rules.md")) {
        log("Warning: 'rules.md' not found in current directory. Rules section might be empty.", "orange");
    }
    initPrint(buffer);
    log("File created: " + QString::fromStdString(outputFileName), "green");

    // --- STEP 2: 写入需求 ---
    log("Step 2: Writing requirements...", "cyan");
    {
        std::ofstream fout(buffer, std::ios::app);
        // 将 Qt QString 转为 UTF-8 std::string 写入
        fout << reqEdit->toPlainText().toStdString() << "\n\n";
        fout.close();
    }

    // --- STEP 3: 生成目录树 (调用 getFile) ---
    log("Step 3: Generating directory tree...", "cyan");
    {
        std::ofstream fout(buffer, std::ios::app);
        fout << "# 项目目录结构：\n\n";
        fout << "```md\n";
        getFile(pathStr, "", false, buffer, fout, ".");
        fout << "```\n\n";
        fout.close();
    }
    log("Directory tree written.", "green");

    // --- STEP 4: 提取代码 (调用 printCode) ---
    log("Step 4: Extracting source code...", "cyan");
    {
        std::ofstream fout(buffer, std::ios::app);
        fout << "# 项目代码实现：\n\n";

        // 调用原有的 printCode 函数 (注意：原函数包含文件流的开关逻辑)
        printCode(pathStr, "", false, buffer, fout, targetExtensions);

        // printCode 结束时 fout 可能是关闭的或者打开的，强制确保关闭
        if(fout.is_open()) fout.close();
    }
    log("Source code extracted.", "green");

    log("All operations completed successfully!", "magenta");
}
