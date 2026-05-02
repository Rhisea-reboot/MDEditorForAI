#ifndef GETFILE_H
#define GETFILE_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <set>

#define fs filesystem

const std::string T_BRANCH = "├── ";
const std::string T_LAST   = "└── ";
const std::string T_VERT   = "│   ";
const std::string T_EMPTY  = "    ";

// 压缩级别枚举定义（在此定义以避免循环包含问题）
enum CompressionLevel {
    COMPRESS_FULL,        // 完整输出，不压缩
    COMPRESS_SKELETON,    // 骨架模式：保留签名和关键结构，省略函数体
    COMPRESS_INTERFACE    // 仅接口：只保留声明，删除所有实现
};

void getFile(const std::fs::path &p,std::string pre,bool isLast, char buffer[],std::ofstream &fout,const std::fs::path &skipPath, const std::vector<std::string>& ignoreList);
bool copyContent(const std::string& srcPath, const std::string& dstPath);

// [MODIFIED] 增加 extensions、ignoreList、压缩级别和去重复参数
void printCode(const std::fs::path &p,std::string pre,bool isLast, char buffer[],std::ofstream &fout,
               const std::vector<std::string>& extensions, const std::vector<std::string>& ignoreList,
               CompressionLevel compLevel, bool skipDupIncludes, std::set<std::string>& seenIncludes);

#endif