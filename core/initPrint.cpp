#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <regex>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <numeric>
#include "initPrint.h"
#include "getFile.h"

// ============ 工具函数 ============

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static std::string toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// ============ 文件扩展名检查 ============

// 判断文件是否需要压缩（仅代码文件）
// 支持的语言：C/C++、Java、Python、JavaScript/TypeScript、Go、Rust、C#、Swift、Kotlin、Scala、Ruby、PHP
bool shouldCompressFile(const std::string& extension) {
    static const std::set<std::string> codeExtensions = {
        // C/C++ 家族
        ".cpp", ".h", ".hpp", ".c", ".cc", ".cxx",
        // Java 家族
        ".java", ".kt", ".scala",
        // 动态语言
        ".py", ".rb", ".php",
        // Web/Node.js
        ".js", ".ts", ".jsx", ".tsx", ".mjs",
        // 系统/现代语言
        ".go", ".rs", ".cs", ".swift",
        // 其他常见语言
        ".r", ".jl", ".lua"
    };
    return codeExtensions.count(toLower(extension)) > 0;
}

// ============ C++ 花括号状态机压缩 ============

enum class LineKind {
    SIGNATURE,      // 函数/类签名
    OPEN_BRACE,     // {
    CLOSE_BRACE,    // }
    PREPROCESSOR,   // #include, #define
    DOC_COMMENT,    // ///, //!, /** */
    COMMENT,        // //, /*
    OTHER
};

static bool isDocComment(const std::string& line) {
    std::string t = trim(line);
    if (t.length() >= 3) {
        if (t.substr(0, 3) == "///" || t.substr(0, 3) == "//!") return true;
        if (t.length() >= 4 && t.substr(0, 4) == "/** ") return true;
    }
    return false;
}

static LineKind classifyCppLine(const std::string& line) {
    std::string t = trim(line);
    
    if (t.empty()) return LineKind::OTHER;
    
    // 预处理器
    if (t[0] == '#') return LineKind::PREPROCESSOR;
    
    // 文档注释
    if (isDocComment(line)) return LineKind::DOC_COMMENT;
    
    // 普通注释 - 不作为签名处理
    if (t.length() >= 2 && (t.substr(0, 2) == "//" || t.substr(0, 2) == "/*")) return LineKind::COMMENT;
    
    // 花括号
    if (t == "{" || t == "}") {
        return (t == "{") ? LineKind::OPEN_BRACE : LineKind::CLOSE_BRACE;
    }
    if (t.back() == '{') return LineKind::OPEN_BRACE;
    if (t[0] == '}') return LineKind::CLOSE_BRACE;
    
    // 签名检测：更宽松的类型关键字开头
    // 检测：类型 + 标识符 + ( 或 {
    static const std::regex sigRe(
        R"(^\s*(class|struct|enum|union|template|inline|virtual|explicit|static|const|constexpr|void|auto|bool|char|int|long|short|unsigned|signed|float|double|size_t|string|vector|map|set|unordered_map|unique_ptr|shared_ptr|std::\w+|QString|Q[A-Z]\w+|Eigen\w+)\s+[A-Za-z_][A-Za-z0-9_:]*\s*[({;])",
        std::regex::optimize
    );
    
    if (std::regex_search(t, sigRe)) {
        return LineKind::SIGNATURE;
    }
    
    // 检查是否是函数/方法的参数行（缩进 + 类型 + 标识符）
    if (!t.empty() && t[0] != '#' && t[0] != '/' && t[0] != '{') {
        // 检测多行函数参数的续行（以类型开头或 ) 结尾）
        static const std::regex paramRe(
            R"(^\s*(const\s+)?[A-Za-z_][A-Za-z0-9_:]*\s*[&*]?\s*[A-Za-z_][A-Za-z0-9_]*\s*[,=)]?\s*$)",
            std::regex::optimize
        );
        if (std::regex_match(t, paramRe)) {
            return LineKind::SIGNATURE;
        }
    }
    
    return LineKind::OTHER;
}

// 改进的骨架压缩：保留完整语法结构
// 原则：要么保留完整签名+{}，要么整段删除，不保留半截
static std::string compressCppSkeleton(const std::string& raw) {
    std::istringstream in(raw);
    std::string line;
    std::vector<std::string> out;
    
    int braceDepth = 0;
    bool inBody = false;
    bool inSignature = false;     // 正在收集多行签名
    bool lastWasSignature = false; // 上一行是签名的一部分
    
    while (std::getline(in, line)) {
        std::string t = trim(line);
        auto kind = classifyCppLine(line);
        
        // 空行保留（限制连续）
        if (t.empty()) {
            if (out.empty() || !out.back().empty()) {
                out.push_back(line);
            }
            lastWasSignature = false;
            continue;
        }
        
        // 预处理器：直接保留
        if (t[0] == '#') {
            out.push_back(line);
            lastWasSignature = false;
            continue;
        }
        
        // 文档注释：保留
        if (isDocComment(line)) {
            out.push_back(line);
            lastWasSignature = false;
            continue;
        }
        
        // 在函数体内部
        if (inBody) {
            // 计算花括号深度变化
            for (char c : t) {
                if (c == '{') braceDepth++;
                if (c == '}') braceDepth--;
            }
            if (braceDepth <= 0) {
                inBody = false;
                // 保留闭合花括号
                out.push_back(line);
            }
            // 否则：函数体内的所有内容都丢弃
            continue;
        }
        
        // 检测签名开始或继续
        if (kind == LineKind::SIGNATURE || inSignature) {
            // 签名行，保留
            out.push_back(line);
            lastWasSignature = true;
            
            // 检查是否以分号结束（声明）
            if (!t.empty() && t.back() == ';') {
                // 声明结束，不是函数定义
                inSignature = false;
            }
            // 检查是否包含 {（单行定义）
            else if (t.find('{') != std::string::npos) {
                // 这是一个定义，检查是否在同一行闭合
                int braceCount = 0;
                for (char c : t) {
                    if (c == '{') braceCount++;
                    if (c == '}') braceCount--;
                }
                if (braceCount > 0) {
                    // 有未闭合的 {，进入 body 模式
                    inBody = true;
                    braceDepth = braceCount;
                    // 移除原有的 { 所在行末尾，添加省略标记
                    if (!out.empty()) {
                        // 保留签名，但清理行尾
                        std::string& last = out.back();
                        // 找到 { 的位置
                        size_t bracePos = last.find('{');
                        if (bracePos != std::string::npos) {
                            // 截断到 { 之前，添加 {}
                            last = last.substr(0, bracePos) + "{};";
                        }
                    }
                    // 等待 } 来闭合
                }
                // 否则是空定义 {}，已经处理了
            }
            // 检查是否是参数续行（下一行可能还有参数或 )
            else {
                inSignature = true;
                // 继续读取直到遇到 { 或 ;
            }
            continue;
        }
        
        // 单独的行首花括号
        if (t == "{" || t.back() == '{') {
            if (lastWasSignature) {
                // 函数/类体开始
                // 不输出这一行，而是修改上一行为 {}
                if (!out.empty()) {
                    std::string& last = out.back();
                    // 找到行尾，替换为 ;
                    size_t lastNonSpace = last.find_last_not_of(" \t");
                    if (lastNonSpace != std::string::npos) {
                        // 如果上一行不以 ; 结尾，添加
                        if (last[lastNonSpace] != ';') {
                            last = last.substr(0, lastNonSpace + 1) + ";";
                        }
                    }
                }
                // 进入 body 模式，直到遇到匹配的 }
                inBody = true;
                braceDepth = 1;
                lastWasSignature = false;
                continue;
            }
        }
        
        // 普通注释：选择性保留
        if (kind == LineKind::COMMENT) {
            if (t.find("===") == std::string::npos && 
                t.find("---") == std::string::npos &&
                t.find("***") == std::string::npos) {
                if (t.length() < 100) {
                    out.push_back(line);
                }
            }
            lastWasSignature = false;
            continue;
        }
        
        // 其他内容：保留（可能是全局变量、命名空间内容等）
        out.push_back(line);
        lastWasSignature = false;
    }
    
    return out.empty() ? "" : 
           std::accumulate(out.begin(), out.end(), std::string(),
                          [](const std::string& a, const std::string& b) { 
                              return a.empty() ? b : a + "\n" + b; 
                          });
}

static std::string compressCppInterface(const std::string& raw) {
    // 接口模式：只保留签名和文档注释
    std::istringstream in(raw);
    std::string line;
    std::vector<std::string> out;
    
    while (std::getline(in, line)) {
        auto kind = classifyCppLine(line);
        if (kind == LineKind::SIGNATURE || kind == LineKind::DOC_COMMENT ||
            kind == LineKind::PREPROCESSOR) {
            out.push_back(line);
        }
    }
    
    return out.empty() ? "" : 
           std::accumulate(out.begin(), out.end(), std::string(),
                          [](const std::string& a, const std::string& b) { 
                              return a.empty() ? b : a + "\n" + b; 
                          });
}

// ============ Ruby/PHP 风格语言压缩（end 关键字闭合） ============

static std::string compressRubyLikeSkeleton(const std::string& raw) {
    std::istringstream in(raw);
    std::string line;
    std::vector<std::string> out;
    
    int blockDepth = 0;           // end 关键字深度
    bool inDefinition = false;    // 在类/函数/模块定义内
    
    while (std::getline(in, line)) {
        std::string t = trim(line);
        
        // 空行保留（限制连续）
        if (t.empty()) {
            if (out.empty() || !out.back().empty()) {
                out.push_back(line);
            }
            continue;
        }
        
        // 检测定义开始（Ruby: class/def/module, PHP: class/function）
        bool isDefStart = false;
        if (t.length() >= 4 && t.substr(0, 4) == "def ") {
            isDefStart = true;  // Ruby/Python 方法
        } else if (t.length() >= 6 && t.substr(0, 6) == "class ") {
            isDefStart = true;  // Ruby/PHP 类
        } else if (t.length() >= 7 && t.substr(0, 7) == "module ") {
            isDefStart = true;  // Ruby 模块
        } else if (t.length() >= 9 && (t.substr(0, 9) == "function " || t.substr(0, 9) == "function(")) {
            isDefStart = true;  // PHP 函数
        } else if (t.find("function ") != std::string::npos && t.find("(") != std::string::npos) {
            isDefStart = true;  // PHP 函数其他形式
        }
        
        if (isDefStart) {
            out.push_back(line);
            inDefinition = true;
            blockDepth = 1;
            continue;
        }
        
        // 在定义内部
        if (inDefinition) {
            // 检测嵌套定义（增加深度）
            if (t.substr(0, 6) == "class " || t.substr(0, 4) == "def " ||
                t.substr(0, 7) == "module " || t.substr(0, 9) == "function ") {
                blockDepth++;
            }
            // 检测 end 关键字（减少深度）
            else if (t == "end" || t.substr(0, 4) == "end " ||
                     (t.size() >= 1 && t[0] == '}')) {  // PHP 也支持 {}
                blockDepth--;
                if (blockDepth <= 0) {
                    inDefinition = false;
                    out.push_back(line);  // 保留 end/}
                }
            }
            // 保留文档注释（Ruby: =begin/=end, RDoc: ##, PHP: /** //)
            else if (t.substr(0, 2) == "##" || t.substr(0, 3) == "###" ||
                     t.substr(0, 4) == "# **" || t.substr(0, 3) == "# @") {
                out.push_back(line);
            }
            // 其他内容丢弃
            continue;
        }
        
        // 函数体外：保留顶层常量、require/include
        if (t.substr(0, 8) == "require " || t.substr(0, 8) == "require_" ||
            t.substr(0, 8) == "include " || t.substr(0, 7) == "import ") {
            // 保留导入语句（转为注释形式）
            // out.push_back("# " + t);
        }
        // 保留常量定义（全大写）
        else if (t.find("=") != std::string::npos) {
            size_t eqPos = t.find('=');
            std::string name = trim(t.substr(0, eqPos));
            // 检查是否全大写（Ruby/PHP 常量习惯）
            bool isConst = std::all_of(name.begin(), name.end(), [](char c) {
                return std::isupper(c) || c == '_';
            }) && !name.empty();
            
            if (isConst) {
                out.push_back(line);
            }
        }
        // 其他顶层代码丢弃（避免碎片）
    }
    
    return out.empty() ? "" : 
           std::accumulate(out.begin(), out.end(), std::string(),
                          [](const std::string& a, const std::string& b) { 
                              return a.empty() ? b : a + "\n" + b; 
                          });
}

// ============ Python 简单结构感知压缩（无外部依赖） ============

static std::string compressPySkeleton(const std::string& raw) {
    std::istringstream in(raw);
    std::string line;
    std::vector<std::string> out;
    
    bool inFunction = false;
    std::string currentIndent;
    
    while (std::getline(in, line)) {
        std::string t = trim(line);
        
        size_t indentEnd = line.find_first_not_of(" \t");
        std::string indent = (indentEnd == std::string::npos) ? "" : line.substr(0, indentEnd);
        int currentLevel = indent.length() / 4; // 假设4空格缩进
        
        // 跳过空行
        if (t.empty()) {
            if (out.empty() || !out.back().empty()) {
                out.push_back(line);
            }
            continue;
        }
        
        // 跳过装饰器和文档字符串标记
        if (t.substr(0, 1) == "@") {
            out.push_back(line);
            continue;
        }
        
        // 检测函数/类定义（确保长度足够）
        bool isDef = t.length() >= 5 && t.substr(0, 4) == "def " && t.back() == ':';
        bool isClass = t.length() >= 7 && t.substr(0, 6) == "class " && t.back() == ':';
        
        if (isDef || isClass) {
            out.push_back(line);
            inFunction = true;
            currentIndent = indent;
            continue;
        }
        
        // 检测函数体结束（缩进减少）
        if (inFunction && currentLevel <= (int)currentIndent.length() / 4) {
            inFunction = false;
        }
        
        // 在函数体内
        if (inFunction) {
            // 只保留 docstring（三引号开头）
            if (t.substr(0, 3) == "\"\"\"" || t.substr(0, 3) == "'''") {
                // 提取第一行作为注释
                size_t end = t.find(t.substr(0, 3), 3);
                std::string summary = t.substr(3, end != std::string::npos ? end - 3 : 80);
                if (summary.length() > 80) summary = summary.substr(0, 77) + "...";
                if (!summary.empty()) {
                    out.push_back(indent + "# " + summary);
                }
            }
            // 其他内容丢弃
            continue;
        }
        
        // 函数体外：只保留模块级常量（全大写），其他全部丢弃
        if (t.find("=") != std::string::npos) {
            size_t eqPos = t.find('=');
            std::string name = trim(t.substr(0, eqPos));
            // 检查是否全大写（常量）
            bool isConst = std::all_of(name.begin(), name.end(), [](char c) {
                return std::isupper(c) || c == '_';
            }) && !name.empty();
            
            if (isConst) {
                out.push_back(line);
            }
        }
        // import/from 行和孤立的碎片都丢弃
    }
    
    return out.empty() ? "" : 
           std::accumulate(out.begin(), out.end(), std::string(),
                          [](const std::string& a, const std::string& b) { 
                              return a.empty() ? b : a + "\n" + b; 
                          });
}

// ============ 主入口 ============

void initPrint(char buffer[]){
    std::ofstream fout(buffer);
    fout<<"# 身份\n\n";
    fout<<"你是一个编程高手，精通各种编程语言，尤其是C++。你有丰富的项目经验，能够高效地编写、调试和优化代码。\n\n";
    fout<<"# 项目规则\n\n";
    fout.close();
    copyContent("rules.md",buffer);
    fout.open(buffer, std::ios::app);
    fout<<"\n\n";
    fout<<"项目需求：\n\n";
    fout.close();
}

std::string compressCode(const std::string& raw, CompressionLevel level, 
                         bool skipDupIncludes, std::set<std::string>& seenIncludes,
                         const std::string& ext) {
    if (level == COMPRESS_FULL) {
        return raw;
    }
    
    std::string extLower = toLower(ext);
    std::string result;
    
    // 根据文件扩展名选择压缩策略
    if (extLower == ".py") {
        // Python：缩进分析
        result = compressPySkeleton(raw);
    } else if (extLower == ".cpp" || extLower == ".h" || extLower == ".hpp" ||
               extLower == ".c" || extLower == ".cc" || extLower == ".cxx") {
        // C/C++：花括号状态机
        if (level == COMPRESS_INTERFACE) {
            result = compressCppInterface(raw);
        } else {
            result = compressCppSkeleton(raw);
        }
    } else if (extLower == ".java" || extLower == ".js" || extLower == ".ts" ||
               extLower == ".jsx" || extLower == ".tsx" || extLower == ".mjs" ||
               extLower == ".go" || extLower == ".rs" || extLower == ".cs" ||
               extLower == ".swift" || extLower == ".kt" || extLower == ".scala" ||
               extLower == ".r" || extLower == ".jl" || extLower == ".lua") {
        // C-family / 类 C 语法语言：复用 C++ 花括号逻辑
        // 包括：Java、JS/TS/JSX/TSX、Go、Rust、C#、Swift、Kotlin、Scala、R、Julia、Lua
        // 注：这些语言语法结构类似（花括号、类/函数定义、文档注释）
        if (level == COMPRESS_INTERFACE) {
            result = compressCppInterface(raw);
        } else {
            result = compressCppSkeleton(raw);
        }
    } else if (extLower == ".rb" || extLower == ".php") {
        // Ruby/PHP：类 Ruby 语法（end 关键字闭合）
        result = compressRubyLikeSkeleton(raw);
    } else {
        // 其他语言：简单处理
        result = (level == COMPRESS_INTERFACE) ? "" : raw;
    }
    
    // 后处理：跨文件 #include 去重
    if (skipDupIncludes && !result.empty()) {
        std::istringstream in(result);
        std::string line;
        std::vector<std::string> out;
        
        while (std::getline(in, line)) {
            std::string t = trim(line);
            if (t.substr(0, 8) == "#include") {
                if (seenIncludes.find(t) == seenIncludes.end()) {
                    seenIncludes.insert(t);
                    out.push_back(line);
                }
            } else {
                out.push_back(line);
            }
        }
        
        result = out.empty() ? "" : 
                 std::accumulate(out.begin(), out.end(), std::string(),
                                [](const std::string& a, const std::string& b) { 
                                    return a.empty() ? b : a + "\n" + b; 
                                });
    }
    
    return result;
}