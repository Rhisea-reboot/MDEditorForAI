#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include "..\include\getFile.h"

void getFile(const std::fs::path &p,std::string pre,bool isLast, char buffer[],std::ofstream &fout,const std::fs::path &skipPath) {
    if (!std::fs::exists(p)){
        abort();
    }
    if (std::fs::exists(skipPath) && std::fs::equivalent(p, skipPath))
        return;
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
        getFile(i,pre+(isLast?T_EMPTY:T_VERT),i==P.back(),buffer,fout,skipPath);
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

void printCode(const std::fs::path &p,std::string pre,bool isLast, char buffer[],std::ofstream &fout){
    if (!std::fs::exists(p)){
        abort();
    }
    if ((std::fs::path(p).extension() == ".cpp") && (std::fs::is_regular_file(p))){
        fout<<p.filename().string()<<"\n\n";
        fout<<"```cpp\n\n";
        fout.close();
        copyContent(p.string(),buffer);
        fout.open(buffer, std::ios::app);
        fout<<"```\n\n";
    }
    if (!std::fs::is_directory(p)){
        return;
    }
    std::vector<std::fs::path > P;
    for (const auto &entry:std::fs::directory_iterator(p)){
        P.push_back(entry.path());
    }
    for (auto i : P){
        printCode(i,pre+(isLast?T_EMPTY:T_VERT),i==P.back(),buffer,fout);
    }
}