#include "SysYFormatter.h"
#include "SysYParser.h"
#include <sstream>
#include <string>
#include <algorithm>
#include <any>
#include <iostream> // 添加用于调试输出

// 添加缩进
void SysYFormatter::addIndent() {
    for (int i = 0; i < indentLevel; ++i) {
        formattedCode << "  "; // 每级缩进两个空格
    }
}

// 添加空格
void SysYFormatter::addSpace() {
    formattedCode << " ";
}

// 添加换行
void SysYFormatter::addNewline() {
    formattedCode << "\n";
}

// 删除多余空格
std::string SysYFormatter::trimTrailingSpaces(const std::string &line) {
    auto end = line.find_last_not_of(" ");
    return (end == std::string::npos) ? "" : line.substr(0, end + 1);
}

// 获取格式化后的代码
std::string SysYFormatter::getFormattedCode() {
    return formattedCode.str();
}

// 调试函数：输出当前格式化代码
void SysYFormatter::printDebugInfo(const std::string &message) {
    std::cerr << "DEBUG: " << message << std::endl;
    std::cerr << "Current formatted code:" << std::endl;
    std::cerr << formattedCode.str() << std::endl;
}

// 访问常量声明
std::any SysYFormatter::visitConstDecl(SysYParser::ConstDeclContext *ctx) {
    printDebugInfo("Visiting ConstDecl");
    addIndent();
    formattedCode << "const ";
    visit(ctx->bType());
    addSpace();

    bool isFirstConstDef = true;
    for (auto *def : ctx->constDef()) {
        if (!isFirstConstDef) {
            formattedCode << ", ";
        }
        isFirstConstDef = false;
        visit(def);
    }

    formattedCode << ";";
    addNewline();
    return nullptr;
}

// 访问常量定义
std::any SysYFormatter::visitConstDef(SysYParser::ConstDefContext *ctx) {
    printDebugInfo("Visiting ConstDef");
    formattedCode << ctx->IDENT()->getText();
    if (!ctx->constExp().empty()) {
        for (size_t i = 0; i < ctx->constExp().size() - 1; ++i) {
            formattedCode << "[";
            visit(ctx->constExp(i));
            formattedCode << "]";
        }
    }
    formattedCode << " = ";
    visit(ctx->constInitVal());
    return nullptr;
}

// 访问常量初始化值
std::any SysYFormatter::visitConstInitVal(SysYParser::ConstInitValContext *ctx) {
    printDebugInfo("Visiting ConstInitVal");
    if (ctx->constExp()) {
        visit(ctx->constExp());
    } else {
        formattedCode << "{";
        bool isFirst = true;
        for (auto *val : ctx->constInitVal()) {
            if (!isFirst) {
                formattedCode << ", ";
            }
            isFirst = false;
            visit(val);
        }
        formattedCode << "}";
    }
    return nullptr;
}

std::any SysYFormatter::visitInitVal(SysYParser::InitValContext *ctx) {
    printDebugInfo("Visiting InitVal");
    if (ctx->exp()) {
        visit(ctx->exp());
    } else {
        formattedCode << "{";
        bool isFirst = true;
        for (auto *val : ctx->initVal()) {
            if (!isFirst) {
                formattedCode << ", ";
            }
            isFirst = false;
            visit(val);
        }
        formattedCode << "}";
    }
    return nullptr;
}

// 访问变量声明
std::any SysYFormatter::visitVarDecl(SysYParser::VarDeclContext *ctx) {
    printDebugInfo("Visiting VarDecl");
    addIndent();
    visit(ctx->bType());
    //addSpace();

    bool isFirstVarDef = true;
    for (auto *def : ctx->varDef()) {
        if (!isFirstVarDef) {
            formattedCode << ", ";
        }
        isFirstVarDef = false;
        visit(def);
    }

    formattedCode << ";";
    addNewline();
    return nullptr;
}

// 访问变量定义
std::any SysYFormatter::visitVarDef(SysYParser::VarDefContext *ctx) {
    printDebugInfo("Visiting VarDef");
    formattedCode << ctx->IDENT()->getText();
    if (!ctx->constExp().empty()) {
        for (auto *exp : ctx->constExp()) {
            formattedCode << "[";
            visit(exp);
            formattedCode << "]";
        }
    }
    if (ctx->ASSIGN()) {
        formattedCode << " = ";
        visit(ctx->initVal());
    }
    return nullptr;
}

// 访问代码块
std::any SysYFormatter::visitBlock(SysYParser::BlockContext *ctx) {
    printDebugInfo("Visiting Block");
    addIndent();
    if (ctx->blockItem().empty()) {
        formattedCode << "{}";
        addNewline();
        return nullptr;
    }

    formattedCode << "{";
    addNewline();
    ++indentLevel;

    for (auto *item : ctx->blockItem()) {
        visit(item);
    }

    --indentLevel;
    addIndent();
    formattedCode << "}";
    addNewline();
    return nullptr;
}

// 访问语句
std::any SysYFormatter::visitStmt(SysYParser::StmtContext *ctx) {
    printDebugInfo("Visiting Stmt");
    addIndent();
    if (ctx->block()) {
        visit(ctx->block());
    } else if (ctx->lVal()) {
        visit(ctx->lVal());
        formattedCode << " = ";
        visit(ctx->exp());
        formattedCode << ";";
        addNewline();
    } else if (ctx->RETURN()) {
        formattedCode << "return";
        if (ctx->exp()) {
            addSpace();
            visit(ctx->exp());
        }
        formattedCode << ";";
        addNewline();
    } else if (ctx->IF()) {
        formattedCode << "if (";
        visit(ctx->cond());
        formattedCode << ") ";
        visit(ctx->stmt(0));
        if (ctx->ELSE()) {
            if (ctx->stmt(1)->IF()) {
                formattedCode << "else if (";
                visit(ctx->stmt(1)->cond());
                formattedCode << ") ";
                visit(ctx->stmt(1)->stmt(0));
            } else {
                formattedCode << "else ";
                visit(ctx->stmt(1));
            }
        }
    } else if (ctx->WHILE()) {
        formattedCode << "while (";
        visit(ctx->cond());
        formattedCode << ") ";
        visit(ctx->stmt(0));
    } else if (ctx->BREAK()) {
        formattedCode << "break;";
        addNewline();
    } else if (ctx->CONTINUE()) {
        formattedCode << "continue;";
        addNewline();
    } else if (ctx->exp()) {
        visit(ctx->exp());
        formattedCode << ";";
        addNewline();
    }
    return nullptr;
}

// 访问表达式
std::any SysYFormatter::visitExp(SysYParser::ExpContext *ctx) {
    printDebugInfo("Visiting Exp");
    visit(ctx->addExp());
    return nullptr;
}

// 在 visitExp 之后添加：
std::any SysYFormatter::visitConstExp(SysYParser::ConstExpContext *ctx) {
    printDebugInfo("Visiting ConstExp");
    visit(ctx->addExp());
    return nullptr;
}

// 添加其他关键的表达式访问函数：
std::any SysYFormatter::visitAddExp(SysYParser::AddExpContext *ctx) {
    printDebugInfo("Visiting AddExp");
    if (ctx->mulExp()) {
        visit(ctx->mulExp());
    } else {
        visit(ctx->addExp());
        formattedCode << " " << (ctx->PLUS() ? "+" : "-") << " ";
        visit(ctx->mulExp());
    }
    return nullptr;
}

std::any SysYFormatter::visitMulExp(SysYParser::MulExpContext *ctx) {
    printDebugInfo("Visiting MulExp");
    if (ctx->unaryExp()) {
        visit(ctx->unaryExp());
    } else {
        visit(ctx->mulExp());
        formattedCode << " ";
        if (ctx->MUL()) formattedCode << "*";
        else if (ctx->DIV()) formattedCode << "/";
        else formattedCode << "%";
        formattedCode << " ";
        visit(ctx->unaryExp());
    }
    return nullptr;
}

std::any SysYFormatter::visitUnaryExp(SysYParser::UnaryExpContext *ctx) {
    printDebugInfo("Visiting UnaryExp");
    if (ctx->primaryExp()) {
        visit(ctx->primaryExp());
    } else if (ctx->IDENT()) {
        formattedCode << ctx->IDENT()->getText() << "(";
        if (ctx->funcRParams()) {
            visit(ctx->funcRParams());
        }
        formattedCode << ")";
    } else {
        visit(ctx->unaryOp());
        visit(ctx->unaryExp());
    }
    return nullptr;
}

// 访问条件表达式
std::any SysYFormatter::visitCond(SysYParser::CondContext *ctx) {
    printDebugInfo("Visiting Cond");
    visit(ctx->lOrExp());
    return nullptr;
}

// 访问左值
std::any SysYFormatter::visitLVal(SysYParser::LValContext *ctx) {
    printDebugInfo("Visiting LVal");
    formattedCode << ctx->IDENT()->getText();
    for (auto *exp : ctx->exp()) {
        formattedCode << "[";
        visit(exp);
        formattedCode << "]";
    }
    return nullptr;
}

// 访问基本表达式
std::any SysYFormatter::visitPrimaryExp(SysYParser::PrimaryExpContext *ctx) {
    printDebugInfo("Visiting PrimaryExp");
    if (ctx->L_PAREN()) {
        formattedCode << "(";
        visit(ctx->exp());
        formattedCode << ")";
    } else if (ctx->lVal()) {
        visit(ctx->lVal());
    } else if (ctx->number()) {
        visit(ctx->number());
    }
    return nullptr;
}

// 访问数字
std::any SysYFormatter::visitNumber(SysYParser::NumberContext *ctx) {
    printDebugInfo("Visiting Number");
    formattedCode << ctx->INTEGER_CONST()->getText();
    return nullptr;
}