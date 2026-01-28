#ifndef UIHELPER_H
#define UIHELPER_H

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// 使用命名空间封装UI功能
namespace UI {

    // ANSI 颜色代码
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string CYAN    = "\033[36m";
    const std::string BOLD    = "\033[1m";

    // 打印带有边框的欢迎 Banner
    inline void printBanner() {
        std::cout << CYAN << BOLD;
        std::cout << "===========================================" << "\n";
        std::cout << "       MD Editor For AI - Project Helper   " << "\n";
        std::cout << "===========================================" << "\n";
        std::cout << RESET << "\n";
    }

    // 打印步骤标题
    inline void printStep(const std::string& title) {
        std::cout << "\n" << YELLOW << BOLD << "[STEP] " << title << RESET << "\n";
    }

    // 打印输入提示
    inline void printPrompt(const std::string& msg) {
        std::cout << GREEN << "[?] " << msg << RESET;
    }

    // 打印错误信息
    inline void printError(const std::string& msg) {
        std::cout << RED << "[!] Error: " << msg << RESET << "\n";
    }

    // 打印成功信息
    inline void printSuccess(const std::string& msg) {
        std::cout << CYAN << "[OK] " << msg << RESET << "\n";
    }

    // 简单的加载动画效果
    inline void showLoading(const std::string& msg) {
        std::cout << msg << std::flush;
        for(int i = 0; i < 3; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            std::cout << "." << std::flush;
        }
        std::cout << "\n";
    }
}

#endif