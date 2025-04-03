grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' (statement)* return_stmt '}' ;

statement:  if_stmt
        |   decl_stmt
        |   assign_stmt
        |   incrdecr_stmt
        ;

decl_stmt: TYPE VAR ('=' expr)? (',' VAR ('=' expr)?)* ';' ;
assign_stmt: VAR '=' expr ';' ;
incrdecr_stmt:  VAR OP=('++' | '--') ';'
            |   OP=('++' | '--') VAR ';'
            ;
if_stmt: if_block (else_block)? ;
if_block: 'if' '(' if_expr_block ')' '{' if_stmt_block '}' ;
if_expr_block: expr ;
if_stmt_block: (statement)* return_stmt? ;
else_block: 'else' '{' (statement)* return_stmt? '}' ;

return_stmt: RETURN expr ';' ;

expr:   CONST                                               #const
    |   VAR                                                 #var
    |   '(' expr ')'                                        #par
    |   VAR OP=('++' | '--')                                #post
    |   OP=('++' | '--') VAR                                #pre
    |   OP=('!' | '-' | '~') expr                           #unary
    |   expr OP=('*' | '/' | '%') expr                      #muldiv
    |   expr OP=('+' | '-') expr                            #addsub
    |   expr OP=('|' | '&' | '^') expr                      #bitwise
    |   expr OP=('==' | '!=' | '<' | '>' | '<=' | '>=') expr #comp
    |   VAR '=' expr                                        #assign
    ;

OPU:    ('++' | '--');

RETURN : 'return' ;
TYPE : 'int';

VAR :   [a-zA-Z][a-zA-Z0-9_]*;
CONST : '-'? [0-9]+ ;

COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
