grammar NumExpr;

expr:   '-' expr
    |   '(' expr ')'
    |   expr '*' expr
    |   expr '+' expr
    |   expr '-' expr
    |   expr '/' expr
    |   expr '^' expr
    |   expr '%' expr
    |   'a' '(' expr ')'
    |   'r' '(' expr ')'
    |   'e' '(' expr ')'
    |   'l' '(' expr ')'
    |   's' '(' expr ')'
    |   'c' '(' expr ')'
    |   Const
    ;

Const : [0-9|/\d+\.\d*|\.?\d+/] +;

WS  :  [ \t\r\n\u000C]+ -> skip;
