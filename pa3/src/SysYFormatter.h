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

    // 起始规则
    std::any visitCompUnit(SysYParser::CompUnitContext *ctx) override;

    // 声明规则
    std::any visitDecl(SysYParser::DeclContext *ctx) override;
    std::any visitConstDecl(SysYParser::ConstDeclContext *ctx) override;
    std::any visitBType(SysYParser::BTypeContext *ctx) override;
    std::any visitConstDef(SysYParser::ConstDefContext *ctx) override;
    std::any visitConstInitVal(SysYParser::ConstInitValContext *ctx) override;
    std::any visitVarDecl(SysYParser::VarDeclContext *ctx) override;
    std::any visitVarDef(SysYParser::VarDefContext *ctx) override;
    std::any visitInitVal(SysYParser::InitValContext *ctx) override;

    // 函数定义规则
    std::any visitFuncDef(SysYParser::FuncDefContext *ctx) override;
    std::any visitFuncType(SysYParser::FuncTypeContext *ctx) override;
    std::any visitFuncFParams(SysYParser::FuncFParamsContext *ctx) override;
    std::any visitFuncFParam(SysYParser::FuncFParamContext *ctx) override;

    // 代码块规则
    std::any visitBlock(SysYParser::BlockContext *ctx) override;
    std::any visitBlockItem(SysYParser::BlockItemContext *ctx) override;

    // 语句规则
    std::any visitStmt(SysYParser::StmtContext *ctx) override;

    // 表达式规则
    std::any visitExp(SysYParser::ExpContext *ctx) override;
    std::any visitCond(SysYParser::CondContext *ctx) override;
    std::any visitLVal(SysYParser::LValContext *ctx) override;
    std::any visitPrimaryExp(SysYParser::PrimaryExpContext *ctx) override;
    std::any visitNumber(SysYParser::NumberContext *ctx) override;
    std::any visitUnaryExp(SysYParser::UnaryExpContext *ctx) override;
    std::any visitUnaryOp(SysYParser::UnaryOpContext *ctx) override;
    std::any visitFuncRParams(SysYParser::FuncRParamsContext *ctx) override;
    std::any visitMulExp(SysYParser::MulExpContext *ctx) override;
    std::any visitAddExp(SysYParser::AddExpContext *ctx) override;
    std::any visitRelExp(SysYParser::RelExpContext *ctx) override;
    std::any visitEqExp(SysYParser::EqExpContext *ctx) override;
    std::any visitLAndExp(SysYParser::LAndExpContext *ctx) override;
    std::any visitLOrExp(SysYParser::LOrExpContext *ctx) override;
    std::any visitConstExp(SysYParser::ConstExpContext *ctx) override;
};

#endif // SYSYFORMATTER_H