#ifndef SYSYFORMATTER_H
#define SYSYFORMATTER_H

#include "SysYParserBaseVisitor.h"
#include <string>
#include <any>
#include <sstream>

class SysYFormatter : public SysYParserBaseVisitor {
private:
    std::ostringstream formattedCode; // 存储格式化后的代码
    int indentLevel = 0;              // 当前缩进级别

    // 添加缩进
    void addIndent();

    // 添加空格
    void addSpace();

    // 添加换行
    void addNewline();

    // 删除多余空格
    std::string trimTrailingSpaces(const std::string &line);

public:
    // 获取格式化后的代码
    std::string getFormattedCode();

    void printDebugInfo(const std::string &message);

    // 访问常量初始化值
    std::any visitConstInitVal(SysYParser::ConstInitValContext *ctx) override;
    // 访问初始化值
    std::any visitInitVal(SysYParser::InitValContext *ctx) override;

    // 访问常量声明
    std::any visitConstDecl(SysYParser::ConstDeclContext *ctx) override;
    // 访问常量定义
    std::any visitConstDef(SysYParser::ConstDefContext *ctx) override;

    // 访问变量声明
    std::any visitVarDecl(SysYParser::VarDeclContext *ctx) override;

    // 访问变量定义
    std::any visitVarDef(SysYParser::VarDefContext *ctx) override;

    // 访问代码块
    std::any visitBlock(SysYParser::BlockContext *ctx) override;

    // 访问语句
    std::any visitStmt(SysYParser::StmtContext *ctx) override;

    // 访问表达式
    std::any visitExp(SysYParser::ExpContext *ctx) override;
    
    std::any visitConstExp(SysYParser::ConstExpContext *ctx) override;
    std::any visitAddExp(SysYParser::AddExpContext *ctx) override;
    std::any visitMulExp(SysYParser::MulExpContext *ctx) override;
    std::any visitUnaryExp(SysYParser::UnaryExpContext *ctx) override;
    // 访问条件表达式
    std::any visitCond(SysYParser::CondContext *ctx) override;

    // 访问左值
    std::any visitLVal(SysYParser::LValContext *ctx) override;

    // 访问基本表达式
    std::any visitPrimaryExp(SysYParser::PrimaryExpContext *ctx) override;

    // 访问数字
    std::any visitNumber(SysYParser::NumberContext *ctx) override;
};

#endif // SYSYFORMATTER_H