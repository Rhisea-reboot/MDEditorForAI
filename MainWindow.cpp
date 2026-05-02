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
#include <set>

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

    // 压缩模式选择
    QHBoxLayout *compressLayout = new QHBoxLayout();
    compressCombo = new QComboBox(this);
    compressCombo->addItem("Full (完整输出)", 0);
    compressCombo->addItem("Skeleton (骨架模式)", 1);
    compressCombo->addItem("Interface Only (仅接口)", 2);
    compressCombo->setCurrentIndex(1); // 默认骨架模式
    compressCombo->setToolTip("选择代码压缩级别：\n- 完整输出：不压缩\n- 骨架模式：省略函数体实现\n- 仅接口：只保留声明");

    // 去重复#include
    dedupCheck = new QCheckBox("去重复 #include", this);
    dedupCheck->setChecked(true);
    dedupCheck->setToolTip("跨文件去除重复的 #include 语句");

    compressLayout->addWidget(compressCombo);
    compressLayout->addWidget(dedupCheck);
    compressLayout->addStretch();
    formLayout->addRow("Compression:", compressLayout);

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
    settings.setValue("compressionLevel", compressCombo->currentIndex());
    settings.setValue("dedupIncludes", dedupCheck->isChecked());
}

void MainWindow::loadSettings()
{
    QSettings settings("MyCompany", "MDEditorForAI");
    nameEdit->setText(settings.value("projectName", "").toString());
    pathEdit->setText(settings.value("targetPath", "").toString());
    extEdit->setText(settings.value("extensions", "cpp h hpp").toString()); // 默认值
    ignoreEdit->setText(settings.value("ignoreList", ".git .vscode build .idea bin obj").toString()); // 默认忽略
    compressCombo->setCurrentIndex(settings.value("compressionLevel", 1).toInt()); // 默认骨架模式
    dedupCheck->setChecked(settings.value("dedupIncludes", true).toBool()); // 默认开启去重
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

    // 获取压缩设置
    int compIndex = compressCombo->currentIndex();
    CompressionLevel compLevel = static_cast<CompressionLevel>(compIndex);
    bool skipDupIncludes = dedupCheck->isChecked();
    std::set<std::string> seenIncludes;
    
    QString compModeStr;
    switch (compLevel) {
        case COMPRESS_FULL: compModeStr = "Full"; break;
        case COMPRESS_SKELETON: compModeStr = "Skeleton"; break;
        case COMPRESS_INTERFACE: compModeStr = "Interface Only"; break;
    }
    log("Compression mode: " + compModeStr + (skipDupIncludes ? " (dedup ON)" : " (dedup OFF)"), "yellow");

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
        getFile(pathStr, "", false, buffer, fout, ".", ignoreList);
        fout << "```\n\n";
        fout.close();
    }
    log("Directory tree written.", "green");

    // --- STEP 4: 提取代码 (调用 printCode) ---
    log("Step 4: Extracting source code...", "cyan");
    {
        std::ofstream fout(buffer, std::ios::app);
        fout << "# 项目代码实现：\n\n";

        // 调用 printCode 函数，传递所有参数（包括压缩设置）
        printCode(pathStr, "", false, buffer, fout, targetExtensions, ignoreList, compLevel, skipDupIncludes, seenIncludes);

        // printCode 结束时 fout 可能是关闭的或者打开的，强制确保关闭
        if(fout.is_open()) fout.close();
    }
    log("Source code extracted.", "green");

    log("All operations completed successfully!", "magenta");
}
