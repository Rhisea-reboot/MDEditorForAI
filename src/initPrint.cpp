#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include "..\include\initPrint.h"
#include "..\include\getFile.h"

void initPrint(char buffer[]){
    std::ofstream fout(buffer);
    fout<<"#身份\n\n";
    fout<<"你是一个编程高手，精通各种编程语言，尤其是C++。你有丰富的项目经验，能够高效地编写、调试和优化代码。\n\n";
    fout<<"#项目规则\n\n";
    fout.close();
    copyContent("rules.md",buffer);
    fout.open(buffer, std::ios::app);
    fout<<"\n\n";
    fout<<"项目需求：\n\n";
    fout.close();
}