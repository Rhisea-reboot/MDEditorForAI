#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QProgressBar>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 浏览文件夹按钮点击事件
    void onBrowseClicked();
    // 生成按钮点击事件
    void onGenerateClicked();

private:
    // UI 组件声明
    QLineEdit *nameEdit;        // 项目名称输入
    QLineEdit *pathEdit;        // 路径输入
    QLineEdit *extEdit;         // 后缀名输入
    QTextEdit *reqEdit;         // 需求输入
    QPushButton *browseBtn;     // 浏览按钮
    QPushButton *generateBtn;   // 生成按钮
    QTextEdit *logConsole;      // 日志输出区域

    // 辅助函数：向日志区域添加信息
    void log(const QString &msg, const QString &color = "black");

    // 核心业务逻辑封装
    void executeGeneration();
};

#endif // MAINWINDOW_H
