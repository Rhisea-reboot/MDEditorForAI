#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>
#include "getFile.h"
#include "initPrint.h"

// 检查文件名是否匹配通配符模式（支持 * 和 ?）
bool matchWildcard(const std::string& pattern, const std::string& name) {
    // 将通配符模式转换为正则表达式
    std::string regexPattern;
    for (char c : pattern) {
        if (c == '*') {
            regexPattern += ".*";
        } else if (c == '?') {
            regexPattern += ".";
        } else if (c == '.') {
            regexPattern += "\\.";
        } else {
            regexPattern += c;
        }
    }
    try {
        std::regex re(regexPattern, std::regex::icase);
        return std::regex_match(name, re);
    } catch (...) {
        // 如果正则表达式无效，进行简单的字符串比较
        return name == pattern;
    }
}

// 检查路径是否应该被忽略（gitignore 风格）
bool shouldIgnore(const std::fs::path& p, const std::vector<std::string>& ignoreList) {
    if (ignoreList.empty()) return false;

    std::string filename = p.filename().string();

    for (const auto& pattern : ignoreList) {
        if (pattern.empty()) continue;

        // 精确匹配文件名或文件夹名
        if (filename == pattern) {
            return true;
        }

        // 通配符匹配（如 *.log, temp*, *.tmp 等）
        if (pattern.find('*') != std::string::npos || pattern.find('?') != std::string::npos) {
            if (matchWildcard(pattern, filename)) {
                return true;
            }
        }

        // 检查路径中任何部分是否匹配（例如 build/debug 会匹配 build）
        for (const auto& part : p) {
            if (part.string() == pattern) {
                return true;
            }
            // 对路径部分也进行通配符匹配
            if (pattern.find('*') != std::string::npos || pattern.find('?') != std::string::npos) {
                if (matchWildcard(pattern, part.string())) {
                    return true;
                }
            }
        }
    }
    return false;
}

void getFile(const std::fs::path &p,std::string pre,bool isLast, char buffer[],std::ofstream &fout,const std::fs::path &skipPath, const std::vector<std::string>& ignoreList) {
    if (!std::fs::exists(p)){
        abort();
    }
    if (std::fs::exists(skipPath) && std::fs::equivalent(p, skipPath))
        return;

    // 检查是否应该忽略
    if (shouldIgnore(p, ignoreList)) {
        return;
    }

    fout<<pre;
    fout<<(isLast?T_LAST:T_BRANCH);
    fout<<p.filename().string();
    if (std::fs::is_directory(p)){
        fout<<'/';
    }
    fout<<'\n';
    if (!std::fs::is_directory(p)){
        return;
    }
    std::vector<std::fs::path > P;
    for (const auto &entry:std::fs::directory_iterator(p)){
        P.push_back(entry.path());
    }
    for (auto i : P){
        getFile(i,pre+(isLast?T_EMPTY:T_VERT),i==P.back(),buffer,fout,skipPath,ignoreList);
    }
}

bool copyContent(const std::string& srcPath, const std::string& dstPath) {
    // 1. 以二进制模式打开，防止换行符被由于系统不同而自动转换
    std::ifstream src(srcPath, std::ios::binary);
    std::ofstream dst(dstPath, std::ios::binary | std::ios::app);

    if (!src || !dst) {
        return false;
    }

    dst << src.rdbuf();
    src.close();
    dst.close();

    return src && dst; // 返回流的状态
}

// [MODIFIED] 实现动态语言支持、忽略列表和代码压缩
void printCode(const std::fs::path &p,std::string pre,bool isLast, char buffer[],std::ofstream &fout, 
               const std::vector<std::string>& extensions, const std::vector<std::string>& ignoreList,
               CompressionLevel compLevel, bool skipDupIncludes, std::set<std::string>& seenIncludes){
    if (!std::fs::exists(p)){
        abort();
    }

    // 检查是否应该忽略（文件或文件夹）
    if (shouldIgnore(p, ignoreList)) {
        return;
    }

    // 检查当前文件后缀是否在用户允许的列表中
    bool isAllowed = false;
    std::string currentExt = p.extension().string();

    // 简单的线性查找
    for(const auto& ext : extensions) {
        if(currentExt == ext) {
            isAllowed = true;
            break;
        }
    }

    if (isAllowed && std::fs::is_regular_file(p)){
        fout<<p.filename().string()<<"\n\n";

        // 动态生成 Markdown 语言标签 (去掉点，例如 .cpp -> cpp)
        std::string langTag = (currentExt.size() > 0 && currentExt[0] == '.') ? currentExt.substr(1) : currentExt;
        fout<<"```"<< langTag <<"\n";

        fout.close();
        
        // 判断是否需要压缩（仅代码文件）
        if (shouldCompressFile(currentExt) && compLevel != COMPRESS_FULL) {
            // 读取文件内容
            std::ifstream srcFile(p.string(), std::ios::binary);
            if (srcFile) {
                std::string content((std::istreambuf_iterator<char>(srcFile)),
                                      std::istreambuf_iterator<char>());
                srcFile.close();
                
                // 压缩代码（结构感知，传入扩展名）
                std::string compressed = compressCode(content, compLevel, skipDupIncludes, seenIncludes, currentExt);
                
                // 写入压缩后的内容
                std::ofstream dst(buffer, std::ios::binary | std::ios::app);
                dst << compressed;
                dst.close();
            }
        } else {
            // 非代码文件或完整模式：直接复制
            copyContent(p.string(), buffer);
        }
        
        fout.open(buffer, std::ios::app);
        fout<<"\n```\n\n";
    }

    if (!std::fs::is_directory(p)){
        return;
    }
    std::vector<std::fs::path > P;
    for (const auto &entry:std::fs::directory_iterator(p)){
        P.push_back(entry.path());
    }
    for (auto i : P){
        // 递归传递所有参数
        printCode(i,pre+(isLast?T_EMPTY:T_VERT),i==P.back(),buffer,fout, 
                  extensions, ignoreList, compLevel, skipDupIncludes, seenIncludes);
    }
}