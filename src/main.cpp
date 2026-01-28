#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <sstream> // [ADDED] 用于字符串分割
#include <Windows.h>
#include "..\include\getFile.h"
#include "..\include\initPrint.h"
#include "..\include\uiHelper.h"
#include "..\include\folderSelector.h"

using namespace std;

void printDirectoryTree(const std::string& path, char buffer[]);

signed main(int argc, char* argv[]){

    system("cls");
    UI::printBanner();
    SetConsoleCP(936);
    SetConsoleOutputCP(936); 
    string name;
    string path;
    string requirements;
    // [ADDED] 用于存储用户输入的后缀
    string extInput;
    vector<string> targetExtensions;

    char buffer[256];

    // --- 第一步：获取项目名称 ---
    UI::printStep("Project Initialization");
    UI::printPrompt("Please input your project name: ");
    getline(cin, name);
    
    while(name.empty()) {
        UI::printError("Project name cannot be empty!");
        UI::printPrompt("Please input your project name: ");
        getline(cin, name);
    }

    sprintf(buffer, "%s.md", name.c_str());
    
    UI::showLoading("Creating markdown file");
    initPrint(buffer);
    UI::printSuccess("File '" + string(buffer) + "' created successfully.");

    // --- 第二步：获取并验证路径 ---
    UI::printStep("Target Directory Configuration");
    while (true) {
        
        UI::printPrompt("Please input the path your project locates at: ");
        getline(cin, path);

        // 移除路径两端可能存在的引号
        if (path.size() >= 2 && path.front() == '"' && path.back() == '"') {
            path = path.substr(1, path.size() - 2);
        }

        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
            break; 
        } else {
            UI::printError("Path does not exist or is not a directory. Please try again.");
        }
    }

    // --- [NEW FEATURE] 第三步：获取目标语言后缀 ---
    UI::printStep("Language Selection");
    cout << "Supported formats: enter extensions separated by space (e.g., 'cpp h py java')\n"
         << "Press ENTER to use default (cpp, h, hpp)\n";
    UI::printPrompt("Input extensions: ");
    getline(cin, extInput);

    if (extInput.empty()) {
        // 默认配置
        targetExtensions = {".cpp", ".h", ".hpp"};
        UI::printSuccess("Using default extensions: .cpp, .h, .hpp");
    } else {
        // 解析输入
        stringstream ss(extInput);
        string temp;
        while (ss >> temp) {
            // 如果用户输入没有带点，自动补全点
            if (!temp.empty() && temp[0] != '.') {
                temp = "." + temp;
            }
            targetExtensions.push_back(temp);
        }
        string msg = "Targeting extensions: ";
        for(const auto& e : targetExtensions) msg += e + " ";
        UI::printSuccess(msg);
    }

    // --- 第四步：获取需求 ---
    UI::printStep("Requirement Collection");
    cout << "Please give me your requirements in detail.\n" 
         << "I will help you make a complete project plan based on your requirements.\n";
    UI::printPrompt("Input requirements: ");
    getline(cin, requirements);

    ofstream fout(buffer, ios::app);
    fout << requirements << "\n\n";
    fout.close();

    // --- 第五步：生成内容 ---
    system("cls");
    UI::printBanner();
    
    UI::printStep("Analyzing Project Structure...");
    UI::showLoading("Scanning directory tree");
    printDirectoryTree(path, buffer);
    UI::printSuccess("Directory tree generated.");

    UI::printStep("Extracting Source Code...");
    UI::showLoading("Reading code files");
    fout.open(buffer, ios::app);
    fout << "# 项目代码实现：\n\n";
    
    try {
        // [MODIFIED] 传入用户定义的后缀列表
        printCode(path, "", false, buffer, fout, targetExtensions);
    } catch (...) {
        UI::printError("An error occurred while reading files.");
    }
    
    fout.close();
    UI::printSuccess("Source code extracted.");

    cout << "\n";
    UI::printSuccess("All operations completed! Check " + string(buffer) + " for details.");
    system("pause");
    return 0;
}

void printDirectoryTree(const std::string& path, char buffer[]) {
    ofstream fout(buffer, ios::app);
    fout << "# 项目目录结构：\n\n";
    fout << "```md\n";
    getFile(path, "", false, buffer, fout, ".");
    fout << "```\n\n";
    fout.close();
}