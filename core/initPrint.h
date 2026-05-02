#ifndef INITPRINT_H
#define INITPRINT_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <regex>

// 从 getFile.h 引入 CompressionLevel 枚举
#include "getFile.h"

// 初始化输出文件
void initPrint(char buffer[]);

// 代码压缩主函数（结构感知版本）
std::string compressCode(const std::string& raw, CompressionLevel level, 
                         bool skipDupIncludes, std::set<std::string>& seenIncludes,
                         const std::string& ext = "");

// 判断文件是否需要压缩（仅代码文件）
bool shouldCompressFile(const std::string& extension);

#endif // INITPRINT_H