grammar Expr;

expr:   '-' expr
    |   '(' expr ')'
    |   expr '*' expr
    |   expr '+' expr
    |   expr '-' expr
    |   expr '/' expr
    |   expr '^' expr
    |   expr '%' expr
    |   Const
    |   Variable
    ;

Variable : [a-zA-Z] +;
Const : [0-9|/\d+\.\d*|\.?\d+/] +;

WS  :  [ \t\r\n\u000C]+ -> skip;
