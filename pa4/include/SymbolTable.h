#pragma once

#include <map>
#include <string>
#include <vector>
#include <optional>

class SymbolTable {
public:
    SymbolTable();
    
    // 进入新的作用域
    void enterScope();
    
    // 退出当前作用域
    void exitScope();
    
    // 定义或更新当前作用域中的变量
    void define(const std::string& name, int value);
    
    // 查找变量，返回变量的值（如果存在）
    std::optional<int> lookup(const std::string& name) const;
    
private:
    // 使用作用域栈来管理符号表
    // 每个作用域是一个 map，存储变量名到值的映射
    std::vector<std::map<std::string, int>> scopes;
};
