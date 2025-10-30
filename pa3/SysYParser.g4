parser grammar SysYParser;

options {
  tokenVocab = SysYLexer; // 使用 SysYLexer.g4 中定义的终结符
}

// 起始规则
compUnit
  : (decl | funcDef)+
  ;

// 声明规则
decl
  : constDecl
  | varDecl
  ;

constDecl
  : CONST bType constDef (',' constDef)* SEMICOLON
  ;

bType
  : INT
  ;

constDef
  : IDENT ('[' constExp ']')* ASSIGN constInitVal
  ;

constInitVal
  : constExp
  | L_BRACE (constInitVal (',' constInitVal)*)? R_BRACE
  ;

varDecl
  : bType varDef (',' varDef)* SEMICOLON
  ;

varDef
  : IDENT ('[' constExp ']')*
  | IDENT ('[' constExp ']')* ASSIGN initVal
  ;

initVal
  : exp
  | L_BRACE (initVal (',' initVal)*)? R_BRACE
  ;

// 函数定义规则
funcDef
  : funcType IDENT L_PAREN (funcFParams)? R_PAREN block
  ;

funcType
  : VOID
  | INT
  ;

funcFParams
  : funcFParam (',' funcFParam)*
  ;

funcFParam
  : bType IDENT ('[' ']' ('[' exp ']')*)?
  ;

// 代码块规则
block
  : L_BRACE blockItem* R_BRACE
  ;

blockItem
  : decl
  | stmt
  ;

// 语句规则
stmt
  : lVal ASSIGN exp SEMICOLON
  | exp? SEMICOLON
  | block
  | IF L_PAREN cond R_PAREN stmt (ELSE stmt)?
  | WHILE L_PAREN cond R_PAREN stmt
  | BREAK SEMICOLON
  | CONTINUE SEMICOLON
  | RETURN exp? SEMICOLON
  ;

// 表达式规则
exp
  : addExp
  ;

cond
  : lOrExp
  ;

lVal
  : IDENT ('[' exp ']')*
  ;

primaryExp
  : L_PAREN exp R_PAREN
  | lVal
  | number
  ;

number
  : INTEGER_CONST
  ;

unaryExp
  : primaryExp
  | IDENT L_PAREN (funcRParams)? R_PAREN
  | unaryOp unaryExp
  ;

unaryOp
  : PLUS
  | MINUS
  | NOT
  ;

funcRParams
  : exp (',' exp)*
  ;

mulExp
  : unaryExp
  | mulExp (MUL | DIV | MOD) unaryExp
  ;

addExp
  : mulExp
  | addExp (PLUS | MINUS) mulExp
  ;

relExp
  : addExp
  | relExp (LT | GT | LE | GE) addExp
  ;

eqExp
  : relExp
  | eqExp (EQ | NEQ) relExp
  ;

lAndExp
  : eqExp
  | lAndExp AND eqExp
  ;

lOrExp
  : lAndExp
  | lOrExp OR lAndExp
  ;

constExp
  : addExp
  ;