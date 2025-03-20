grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' (statement)* return_stmt '}' ;

statement:  decl_stmt
        |   assign_stmt
        |   incrdecr_stmt
        ;

decl_stmt: TYPE VAR ('=' expr)? (',' VAR ('=' expr)?)* ';' ;
assign_stmt: VAR '=' expr ';' ;
incrdecr_stmt:  VAR OPU ';' #post
            |   OPU VAR ';' #pre
            ;
return_stmt: RETURN expr ';' ;

expr:   '(' expr ')'    #par
    |   expr OPM expr   #muldiv
    |   expr OPA expr   #addsub
    |   expr OPB expr   #bitBybit
    |   VAR             #var
    |   CONST           #const
    ;

OPU:    ('++' | '--');
OPM:    ('*' | '/' | '%') ; 
OPA:    ('+' | '-') ;
OPB:    ('|' | '&' | '^') ;

RETURN : 'return' ;
TYPE : 'int';

VAR :   [a-zA-Z][a-zA-Z0-9_]*;
CONST : '-'? [0-9]+ ;


COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
