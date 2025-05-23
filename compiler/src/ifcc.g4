grammar ifcc;

axiom : prog EOF ;

prog : decl_func_stmt* ;

decl_func_stmt: TYPE VAR '(' (TYPE VAR)? (',' TYPE VAR)* ')' (block | ';');

statement:  if_stmt
        |   while_stmt
        |   decl_stmt
        |   assign_stmt
        |   incrdecr_stmt
        |   call_func_stmt
        |   block
        |   return_stmt
        ;

decl_stmt: TYPE VAR ('[' INTEGER ']')? ('=' expr)? (',' VAR ('[' INTEGER ']')? ('=' expr)?)* ';' ;
assign_stmt: VAR ('[' expr ']')? '=' expr ';' ;
incrdecr_stmt:  VAR OP=('++' | '--') ';' #post_stmt
            |   OP=('++' | '--') VAR ';' #pre_stmt
            ;
if_stmt: if_block (else_block)? ;
if_block: 'if' '(' if_expr_block ')' if_stmt_block ;
if_expr_block: expr ;
if_stmt_block: block;
else_block: 'else' block ;
while_stmt: 'while' '(' while_expr_block ')' while_stmt_block ;
while_expr_block: expr ;
while_stmt_block: block ;

call_func_stmt: VAR '(' (expr)? (',' expr)* ')' (';')?;
return_stmt: RETURN (expr)? ';' ;
block: '{' (statement)* '}' ;

expr:   cst                                               #const
    |   VAR                                                 #var
    |   call_func_stmt                                      #call
    |   VAR '[' expr ']'                                    #array_access
    |   '{' expr (',' expr)* '}'                            #array_init
    |   '(' expr ')'                                        #par
    |   VAR OP=('++' | '--')                                #post
    |   OP=('++' | '--') VAR                                #pre
    |   OP=('!' | '-' | '~') expr                           #unary
    |   expr OP=('*' | '/' | '%') expr                      #muldiv
    |   expr OP=('+' | '-') expr                            #addsub
    |   expr OP=('<<' | '>>') expr                          #shift 
    |   expr OP=('|' | '&' | '^') expr                      #bitwise
    |   expr OP=('==' | '!=' | '<' | '>' | '<=' | '>=') expr #comp
    |   expr '&&' expr                                      #logicalAND
    |   expr '||' expr                                      #logicalOR    
    |   VAR '=' expr                                        #assign
    ;

OPU:    ('++' | '--');

RETURN : 'return' ;
TYPE : 'void' | 'int' | 'char';

VAR :   [a-zA-Z][a-zA-Z0-9_]*;
cst : INTEGER | CHAR ;
INTEGER :  '-'? [0-9]+ ;
CHAR : '\'' . '\'';

COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
