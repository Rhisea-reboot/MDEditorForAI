#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include "..\include\getFile.h"
#include "..\include\initPrint.h"

using namespace std;

void printDirectoryTree(const std::string& path, char buffer[]);

signed main(int argc, char* argv[]){
    string name;
    cout<<"Please input your project name>>>";
    cin>>name;
    char buffer[256];
    sprintf(buffer, "%s.md", name.c_str());
    initPrint(buffer);
    string path;
    getchar();

    cout<<"Please input the path your project locates at>>>";
    getline(cin,path);

    cout<<"Please give me your requirements in detail, I will help you make a complete project plan based on your requirements>>>";
    string requirements;
    getline(cin,requirements);
    system("cls");
    ofstream fout(buffer,ios::app);
    fout<<requirements<<"\n\n";
    fout.close();

    printDirectoryTree(path, buffer);

    fout.open(buffer, ios::app);
    fout<<"#项目代码实现：\n\n";
    printCode(path, "", false, buffer, fout);
    return 0;
}

void printDirectoryTree(const std::string& path, char buffer[]) {
    ofstream fout(buffer, ios::app);
    fout << "#项目目录结构：\n\n";
    fout<<"```md\n";
    getFile(path, "", false, buffer, fout,".");
    fout<<"```\n\n";
    fout.close();
}