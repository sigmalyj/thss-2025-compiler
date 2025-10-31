#include "SysYFormatter.h"
#include "SysYParser.h"
#include <sstream>
#include <string>
#include <algorithm>
#include <any>

// ==================== 工具函数 ====================

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

// ==================== 起始规则 ====================

// 访问编译单元(最顶层)
std::any SysYFormatter::visitCompUnit(SysYParser::CompUnitContext *ctx) {
    for (auto *item : ctx->children) {
        if (auto *decl = dynamic_cast<SysYParser::DeclContext*>(item)) {
            visit(decl);
        } else if (auto *funcDef = dynamic_cast<SysYParser::FuncDefContext*>(item)) {
            visit(funcDef);
        }
    }
    return nullptr;
}

// ==================== 声明规则 ====================

// 访问声明
std::any SysYFormatter::visitDecl(SysYParser::DeclContext *ctx) {
    if (ctx->constDecl()) {
        visit(ctx->constDecl());
    } else if (ctx->varDecl()) {
        visit(ctx->varDecl());
    }
    return nullptr;
}

// 访问常量声明
std::any SysYFormatter::visitConstDecl(SysYParser::ConstDeclContext *ctx) {
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

// 访问基本类型
std::any SysYFormatter::visitBType(SysYParser::BTypeContext *ctx) {
    formattedCode << "int";
    return nullptr;
}

// 访问常量定义
std::any SysYFormatter::visitConstDef(SysYParser::ConstDefContext *ctx) {
    formattedCode << ctx->IDENT()->getText();
    if (!ctx->constExp().empty()) {
        for (auto *exp : ctx->constExp()) {
            formattedCode << "[";
            visit(exp);
            formattedCode << "]";
        }
    }
    formattedCode << " = ";
    visit(ctx->constInitVal());
    return nullptr;
}

// 访问常量初始化值
std::any SysYFormatter::visitConstInitVal(SysYParser::ConstInitValContext *ctx) {
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

// 访问变量声明
std::any SysYFormatter::visitVarDecl(SysYParser::VarDeclContext *ctx) {
    addIndent();
    visit(ctx->bType());
    addSpace();

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

// 访问初始化值
std::any SysYFormatter::visitInitVal(SysYParser::InitValContext *ctx) {
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

// ==================== 函数定义规则 ====================

// 访问函数定义
std::any SysYFormatter::visitFuncDef(SysYParser::FuncDefContext *ctx) {
    addIndent();
    visit(ctx->funcType());
    addSpace();
    formattedCode << ctx->IDENT()->getText() << "(";
    if (ctx->funcFParams()) {
        visit(ctx->funcFParams());
    }
    formattedCode << ") ";
    visit(ctx->block());
    return nullptr;
}

// 访问函数类型
std::any SysYFormatter::visitFuncType(SysYParser::FuncTypeContext *ctx) {
    if (ctx->VOID()) {
        formattedCode << "void";
    } else if (ctx->INT()) {
        formattedCode << "int";
    }
    return nullptr;
}

// 访问函数形参列表
std::any SysYFormatter::visitFuncFParams(SysYParser::FuncFParamsContext *ctx) {
    bool isFirst = true;
    for (auto *param : ctx->funcFParam()) {
        if (!isFirst) {
            formattedCode << ", ";
        }
        isFirst = false;
        visit(param);
    }
    return nullptr;
}

// 访问函数形参
std::any SysYFormatter::visitFuncFParam(SysYParser::FuncFParamContext *ctx) {
    visit(ctx->bType());
    addSpace();
    formattedCode << ctx->IDENT()->getText();
    
    // 处理数组参数
    if (ctx->L_BRACKT().size() > 0) {
        formattedCode << "[]";
        // 处理多维数组的其他维度
        for (size_t i = 1; i < ctx->L_BRACKT().size(); ++i) {
            formattedCode << "[";
            visit(ctx->exp(i - 1));
            formattedCode << "]";
        }
    }
    return nullptr;
}

// ==================== 代码块规则 ====================

// 访问代码块
std::any SysYFormatter::visitBlock(SysYParser::BlockContext *ctx) {
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

// 访问块项
std::any SysYFormatter::visitBlockItem(SysYParser::BlockItemContext *ctx) {
    if (ctx->decl()) {
        visit(ctx->decl());
    } else if (ctx->stmt()) {
        visit(ctx->stmt());
    }
    return nullptr;
}

// ==================== 语句规则 ====================

// 访问语句
std::any SysYFormatter::visitStmt(SysYParser::StmtContext *ctx) {
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
            addIndent();
            formattedCode << "else ";
            visit(ctx->stmt(1));
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
    } else {
        formattedCode << ";";
        addNewline();
    }
    return nullptr;
}

// ==================== 表达式规则 ====================

// 访问表达式
std::any SysYFormatter::visitExp(SysYParser::ExpContext *ctx) {
    visit(ctx->addExp());
    return nullptr;
}

// 访问条件表达式
std::any SysYFormatter::visitCond(SysYParser::CondContext *ctx) {
    visit(ctx->lOrExp());
    return nullptr;
}

// 访问左值
std::any SysYFormatter::visitLVal(SysYParser::LValContext *ctx) {
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
    formattedCode << ctx->INTEGER_CONST()->getText();
    return nullptr;
}

// 访问一元表达式
std::any SysYFormatter::visitUnaryExp(SysYParser::UnaryExpContext *ctx) {
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

// 访问一元运算符
std::any SysYFormatter::visitUnaryOp(SysYParser::UnaryOpContext *ctx) {
    if (ctx->PLUS()) {
        formattedCode << "+";
    } else if (ctx->MINUS()) {
        formattedCode << "-";
    } else if (ctx->NOT()) {
        formattedCode << "!";
    }
    return nullptr;
}

// 访问函数实参列表
std::any SysYFormatter::visitFuncRParams(SysYParser::FuncRParamsContext *ctx) {
    bool isFirst = true;
    for (auto *exp : ctx->exp()) {
        if (!isFirst) {
            formattedCode << ", ";
        }
        isFirst = false;
        visit(exp);
    }
    return nullptr;
}

// 访问乘除表达式
std::any SysYFormatter::visitMulExp(SysYParser::MulExpContext *ctx) {
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

// 访问加减表达式
std::any SysYFormatter::visitAddExp(SysYParser::AddExpContext *ctx) {
    if (ctx->mulExp()) {
        visit(ctx->mulExp());
    } else {
        visit(ctx->addExp());
        formattedCode << " " << (ctx->PLUS() ? "+" : "-") << " ";
        visit(ctx->mulExp());
    }
    return nullptr;
}

// 访问关系表达式
std::any SysYFormatter::visitRelExp(SysYParser::RelExpContext *ctx) {
    if (ctx->addExp()) {
        visit(ctx->addExp());
    } else {
        visit(ctx->relExp());
        formattedCode << " ";
        if (ctx->LT()) formattedCode << "<";
        else if (ctx->GT()) formattedCode << ">";
        else if (ctx->LE()) formattedCode << "<=";
        else formattedCode << ">=";
        formattedCode << " ";
        visit(ctx->addExp());
    }
    return nullptr;
}

// 访问相等表达式
std::any SysYFormatter::visitEqExp(SysYParser::EqExpContext *ctx) {
    if (ctx->relExp()) {
        visit(ctx->relExp());
    } else {
        visit(ctx->eqExp());
        formattedCode << " ";
        if (ctx->EQ()) formattedCode << "==";
        else formattedCode << "!=";
        formattedCode << " ";
        visit(ctx->relExp());
    }
    return nullptr;
}

// 访问逻辑与表达式
std::any SysYFormatter::visitLAndExp(SysYParser::LAndExpContext *ctx) {
    if (ctx->eqExp()) {
        visit(ctx->eqExp());
    } else {
        visit(ctx->lAndExp());
        formattedCode << " && ";
        visit(ctx->eqExp());
    }
    return nullptr;
}

// 访问逻辑或表达式
std::any SysYFormatter::visitLOrExp(SysYParser::LOrExpContext *ctx) {
    if (ctx->lAndExp()) {
        visit(ctx->lAndExp());
    } else {
        visit(ctx->lOrExp());
        formattedCode << " || ";
        visit(ctx->lAndExp());
    }
    return nullptr;
}

// 访问常量表达式
std::any SysYFormatter::visitConstExp(SysYParser::ConstExpContext *ctx) {
    visit(ctx->addExp());
    return nullptr;
}
