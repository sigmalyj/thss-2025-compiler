#pragma once

#include "CalculatorBaseVisitor.h"
#include "SymbolTable.h"
#include <memory>
#include <sstream>
#include <iostream>

class InterpreterVisitor : public CalculatorBaseVisitor {
public:
    InterpreterVisitor();
    
    // 访问程序
    virtual std::any visitProgram(CalculatorParser::ProgramContext *ctx) override;
    
    // 访问赋值语句
    virtual std::any visitAssignment(CalculatorParser::AssignmentContext *ctx) override;
    
    // 访问代码块
    virtual std::any visitBlock(CalculatorParser::BlockContext *ctx) override;
    
    // 访问打印语句
    virtual std::any visitPrintStmt(CalculatorParser::PrintStmtContext *ctx) override;
    
    // 访问表达式
    virtual std::any visitIntExpr(CalculatorParser::IntExprContext *ctx) override;
    virtual std::any visitLvalExpr(CalculatorParser::LvalExprContext *ctx) override;
    virtual std::any visitParenExpr(CalculatorParser::ParenExprContext *ctx) override;
    virtual std::any visitMulExpr(CalculatorParser::MulExprContext *ctx) override;
    virtual std::any visitPlusExpr(CalculatorParser::PlusExprContext *ctx) override;
    
    // 检查是否发生错误
    bool hasError() const { return errorOccurred; }
    
private:
    SymbolTable symbolTable;
    bool errorOccurred;
};