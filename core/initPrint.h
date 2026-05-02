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
// 支持语言：C/C++、Java、Python、JavaScript/TypeScript/JSX/TSX、Go、Rust、C#、Swift、
//          Kotlin、Scala、Ruby、PHP、R、Julia、Lua
std::string compressCode(const std::string& raw, CompressionLevel level, 
                         bool skipDupIncludes, std::set<std::string>& seenIncludes,
                         const std::string& ext = "");

// 判断文件是否需要压缩（仅代码文件）
// 支持扩展名：.cpp/.h/.hpp/.c/.cc/.cxx, .java/.kt/.scala,
//            .py, .js/.ts/.jsx/.tsx/.mjs, .go/.rs/.cs/.swift,
//            .rb/.php, .r/.jl/.lua
bool shouldCompressFile(const std::string& extension);

#endif // INITPRINT_H