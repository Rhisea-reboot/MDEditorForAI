#ifndef GETFILE_H
#define GETFILE_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

#define fs filesystem

const std::string T_BRANCH = "├── ";
const std::string T_LAST   = "└── ";
const std::string T_VERT   = "│   ";
const std::string T_EMPTY  = "    ";

void getFile(const std::fs::path &p,std::string pre,bool isLast, char buffer[],std::ofstream &fout,const std::fs::path &skipPath);
bool copyContent(const std::string& srcPath, const std::string& dstPath);

// [MODIFIED] 增加 extensions 参数，用于传递允许的文件后缀列表
void printCode(const std::fs::path &p,std::string pre,bool isLast, char buffer[],std::ofstream &fout, const std::vector<std::string>& extensions);

#endif