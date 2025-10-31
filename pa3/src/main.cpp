#include <iostream>
#include <fstream>
#include "SysYLexer.h"
#include "SysYParser.h"
#include "SysYFormatter.h"
using namespace antlr4;

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: sysy_formatter <input-file>" << std::endl;
        return 1;
    }

    // 打开输入文件
    std::ifstream stream(argv[1]);
    if (!stream.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    // 创建 ANTLR 输入流
    ANTLRInputStream input(stream);

    // 创建词法分析器和标记流
    SysYLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    // 创建语法分析器
    SysYParser parser(&tokens);

    // 获取语法分析树
    tree::ParseTree *tree = parser.compUnit();

    // 创建格式化器并访问语法树
    SysYFormatter formatter;
    formatter.visit(tree);

    // 输出格式化后的代码
    std::cout << formatter.getFormattedCode() << std::endl;

    return 0;
}