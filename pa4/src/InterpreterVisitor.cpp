#include "InterpreterVisitor.h"
#include "CalculatorParser.h"
#include <iostream>

InterpreterVisitor::InterpreterVisitor() : errorOccurred(false) {}

std::any InterpreterVisitor::visitProgram(CalculatorParser::ProgramContext *ctx) {
    // 遍历所有语句
    for (auto stmt : ctx->statement()) {
        visit(stmt);
        // 如果发生错误，停止执行
        if (errorOccurred) {
            break;
        }
    }
    return nullptr;
}

std::any InterpreterVisitor::visitAssignment(CalculatorParser::AssignmentContext *ctx) {
    // 获取变量名
    std::string varName = ctx->ID()->getText();
    
    // 计算表达式的值
    std::any exprResult = visit(ctx->expr());
    
    // 如果发生错误，直接返回
    if (errorOccurred) {
        return nullptr;
    }
    
    int value = std::any_cast<int>(exprResult);
    
    // 在当前作用域中定义变量
    symbolTable.define(varName, value);
    
    return nullptr;
}

std::any InterpreterVisitor::visitBlock(CalculatorParser::BlockContext *ctx) {
    // 进入新的作用域
    symbolTable.enterScope();
    
    // 执行代码块中的所有语句
    for (auto stmt : ctx->statement()) {
        visit(stmt);
        // 如果发生错误，停止执行
        if (errorOccurred) {
            break;
        }
    }
    
    // 退出作用域
    symbolTable.exitScope();
    
    return nullptr;
}

std::any InterpreterVisitor::visitPrintStmt(CalculatorParser::PrintStmtContext *ctx) {
    // 计算表达式的值
    std::any exprResult = visit(ctx->expr());
    
    // 如果发生错误，直接返回
    if (errorOccurred) {
        return nullptr;
    }
    
    int value = std::any_cast<int>(exprResult);
    
    // 打印结果
    std::cout << value << std::endl;
    
    return nullptr;
}

std::any InterpreterVisitor::visitIntExpr(CalculatorParser::IntExprContext *ctx) {
    // 返回整数常量的值
    return std::stoi(ctx->INT()->getText());
}

std::any InterpreterVisitor::visitLvalExpr(CalculatorParser::LvalExprContext *ctx) {
    // 查找变量的值
    std::string varName = ctx->ID()->getText();
    auto value = symbolTable.lookup(varName);
    
    if (!value.has_value()) {
        // 变量未定义，报错
        errorOccurred = true;
        std::cout << "Error: Undefined symbol '" << varName 
                  << "' at line " << ctx->getStart()->getLine() << std::endl;
        return 0;  // 返回一个默认值
    }
    
    return value.value();
}

std::any InterpreterVisitor::visitParenExpr(CalculatorParser::ParenExprContext *ctx) {
    // 返回括号内表达式的值
    return visit(ctx->expr());
}

std::any InterpreterVisitor::visitMulExpr(CalculatorParser::MulExprContext *ctx) {
    // 计算左右操作数
    std::any leftResult = visit(ctx->expr(0));
    if (errorOccurred) return 0;
    
    std::any rightResult = visit(ctx->expr(1));
    if (errorOccurred) return 0;
    
    int left = std::any_cast<int>(leftResult);
    int right = std::any_cast<int>(rightResult);
    
    // 根据操作符执行运算
    std::string op = ctx->children[1]->getText();
    
    if (op == "*") {
        return left * right;
    } else if (op == "/") {
        return left / right;
    } else if (op == "%") {
        return left % right;
    }
    
    return 0;
}

std::any InterpreterVisitor::visitPlusExpr(CalculatorParser::PlusExprContext *ctx) {
    // 计算左右操作数
    std::any leftResult = visit(ctx->expr(0));
    if (errorOccurred) return 0;
    
    std::any rightResult = visit(ctx->expr(1));
    if (errorOccurred) return 0;
    
    int left = std::any_cast<int>(leftResult);
    int right = std::any_cast<int>(rightResult);
    
    // 根据操作符执行运算
    std::string op = ctx->children[1]->getText();
    
    if (op == "+") {
        return left + right;
    } else if (op == "-") {
        return left - right;
    }
    
    return 0;
}
