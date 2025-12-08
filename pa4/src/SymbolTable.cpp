#include "SymbolTable.h"

SymbolTable::SymbolTable() {
    // 初始化时创建全局作用域
    scopes.push_back(std::map<std::string, int>());
}

void SymbolTable::enterScope() {
    // 创建新的作用域
    scopes.push_back(std::map<std::string, int>());
}

void SymbolTable::exitScope() {
    // 退出当前作用域
    if (scopes.size() > 1) {
        scopes.pop_back();
    }
}

void SymbolTable::define(const std::string& name, int value) {
    // 在当前作用域中定义或更新变量
    scopes.back()[name] = value;
}

std::optional<int> SymbolTable::lookup(const std::string& name) const {
    // 从内层作用域向外层作用域查找变量
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second;
        }
    }
    // 变量未定义
    return std::nullopt;
}
