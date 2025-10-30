#include "SysYFormatter.h"
#include <sstream>
#include <string>
#include <algorithm>

class SysYFormatter : public SysYParserBaseVisitor {
private:
    std::ostringstream formattedCode; // 存储格式化后的代码
    int indentLevel = 0;              // 当前缩进级别

    // 添加缩进
    void addIndent() {
        for (int i = 0; i < indentLevel; ++i) {
            formattedCode << "  "; // 每级缩进两个空格
        }
    }

    // 添加空格
    void addSpace() {
        formattedCode << " ";
    }

    // 添加换行
    void addNewline() {
        formattedCode << "\n";
    }

    // 删除多余空格
    std::string trimTrailingSpaces(const std::string &line) {
        auto end = line.find_last_not_of(" ");
        return (end == std::string::npos) ? "" : line.substr(0, end + 1);
    }

public:
    // 获取格式化后的代码
    std::string getFormattedCode() {
        return formattedCode.str();
    }

    // 访问变量声明
    std::any visitVarDecl(SysYParser::VarDeclContext *ctx) override {
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
    std::any visitVarDef(SysYParser::VarDefContext *ctx) override {
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
    std::any visitBlock(SysYParser::BlockContext *ctx) override {
        addIndent();
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
    std::any visitStmt(SysYParser::StmtContext *ctx) override {
        if (ctx->block()) {
            visit(ctx->block());
        } else {
            addIndent();
            if (ctx->lVal()) {
                visit(ctx->lVal());
                formattedCode << " = ";
                visit(ctx->exp());
                formattedCode << ";";
            } else if (ctx->RETURN()) {
                formattedCode << "return";
                if (ctx->exp()) {
                    addSpace();
                    visit(ctx->exp());
                }
                formattedCode << ";";
            } else if (ctx->IF()) {
                formattedCode << "if (";
                visit(ctx->cond());
                formattedCode << ") ";
                visit(ctx->stmt(0));
                if (ctx->ELSE()) {
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
            } else if (ctx->CONTINUE()) {
                formattedCode << "continue;";
            } else if (ctx->exp()) {
                visit(ctx->exp());
                formattedCode << ";";
            }
            addNewline();
        }
        return nullptr;
    }

    // 访问表达式
    std::any visitExp(SysYParser::ExpContext *ctx) override {
        visit(ctx->addExp());
        return nullptr;
    }

    // 访问条件表达式
    std::any visitCond(SysYParser::CondContext *ctx) override {
        visit(ctx->lOrExp());
        return nullptr;
    }

    // 访问左值
    std::any visitLVal(SysYParser::LValContext *ctx) override {
        formattedCode << ctx->IDENT()->getText();
        for (auto *exp : ctx->exp()) {
            formattedCode << "[";
            visit(exp);
            formattedCode << "]";
        }
        return nullptr;
    }

    // 访问基本表达式
    std::any visitPrimaryExp(SysYParser::PrimaryExpContext *ctx) override {
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
    std::any visitNumber(SysYParser::NumberContext *ctx) override {
        formattedCode << ctx->INTEGER_CONST()->getText();
        return nullptr;
    }
};