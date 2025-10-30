lexer grammar SysYLexer;

CONST: 'const';
INT: 'int';
VOID: 'void';
IF: 'if';
ELSE: 'else';
WHILE: 'while';
BREAK: 'break';
CONTINUE: 'continue';
RETURN: 'return';

EQ: '==';
NEQ: '!=';
LE: '<=';
GE: '>=';
AND: '&&';
OR: '||';

ASSIGN: '=';
LT: '<';
GT: '>';
NOT: '!';
PLUS: '+';
MINUS: '-';
MUL: '*';
DIV: '/';
MOD: '%';

L_PAREN: '(';
R_PAREN: ')';
L_BRACE: '{';
R_BRACE: '}';
L_BRACKT: '[';
R_BRACKT: ']';
COMMA: ',';
SEMICOLON: ';';

IDENT: [a-zA-Z_] [a-zA-Z0-9_]*;

INTEGER_CONST:
	'0' [xX] [0-9a-fA-F]+ // 0x... 十六进制
	| [1-9] [0-9]* // 十进制（非零开头）
	| '0' [0-7]* ; // 以0开头的八进制或单个0

WS: [ \t\r\n]+ -> skip;

LINE_COMMENT: '//' ~[\r\n]* -> skip;

MULTILINE_COMMENT: '/*' .*? '*/' -> skip;