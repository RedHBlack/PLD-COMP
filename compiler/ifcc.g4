grammar ifcc;

axiom : prog EOF ;

prog : decl_func_stmt* 'int' 'main' '(' ')' '{' (statement)* return_stmt '}' ;

statement:  decl_stmt
        |   assign_stmt
        |   incrdecr_stmt
        |   call_func_stmt
        |   block
        ;

decl_stmt: TYPE VAR ('=' expr)? (',' VAR ('=' expr)?)* ';' ;
assign_stmt: VAR '=' expr ';' ;
incrdecr_stmt:  VAR OP=('++' | '--') ';'
            |   OP=('++' | '--') VAR ';'
            ;
decl_func_stmt: TYPE VAR '(' (TYPE VAR)+ (',' TYPE VAR)* ')' (block | ';');
call_func_stmt: VAR '(' (expr)+ (',' expr)* ')' ';';
return_stmt: RETURN expr ';' ;
block: '{' (statement)* return_stmt? '}' ;

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
