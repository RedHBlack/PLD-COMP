grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' (statement)* return_stmt '}' ;

statement:  decl_stmt
        |   assign_stmt
        |   incrdecr_stmt
        ;

decl_stmt: TYPE VAR (',' VAR)* ';' ;
assign_stmt: VAR '=' expr ';' ;
incrdecr_stmt:  VAR OPU ';' #post
            |   OPU VAR ';' #pre
            ;
return_stmt: RETURN expr ';' ;

expr:   '!' expr         #not
    |   expr OPM expr   #muldiv
    |   expr OP=('+' | '-') expr   #addsub
    |   VAR             #var
    |   CONST           #const
    |   '(' expr ')'    #par
    |   '-' expr         #neg
    ;

OPU:    ('++' | '--');
OPM:    ('*' | '/' | '%') ; 

RETURN : 'return' ;
TYPE : 'int';

VAR :   [a-zA-Z][a-zA-Z0-9_]*;
CONST : '-'? [0-9]+ ;

COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
