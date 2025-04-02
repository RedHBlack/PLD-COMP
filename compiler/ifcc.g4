grammar ifcc;

axiom : prog EOF ;

prog : TYPE 'main' '(' ')' '{' (statement)* return_stmt '}' ;

statement:  decl_stmt
        |   assign_stmt
        |   incrdecr_stmt
        ;

decl_stmt: TYPE VAR ('[' CONST ']')? ('=' expr)? (',' VAR ('[' CONST ']')? ('=' expr)?)* ';' ;
assign_stmt: VAR ('[' expr ']')? '=' expr ';' ;
incrdecr_stmt:  VAR OP=('++' | '--') ';'
            |   OP=('++' | '--') VAR ';'
            ;
return_stmt: RETURN expr ';' ;

expr:   CONST                                               #const
    |   VAR                                                 #var
    |   VAR '[' expr ']'                                    #array_access
    |   '{' expr (',' expr)* '}'                            #array_init
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
TYPE : 'void' | 'int' ;

VAR :   [a-zA-Z][a-zA-Z0-9_]*;
CONST : '-'? [0-9]+ | '\'' . '\'' ;


COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
