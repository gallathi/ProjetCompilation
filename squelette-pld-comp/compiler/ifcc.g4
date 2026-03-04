grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' declaration bloc return_stmt '}' ;

bloc: affectation bloc | /* epsilon */ ;
affectation: VAR '=' expression ';' ;


expression: '(' expression ')'                  #par
    | expression OPMULTDIV expression           #multdiv
    | expression OPADDSUB expression            #addsub
    | CONST                                     #const
    | VAR                                       #var;

declaration: 'int' declaration_var ';' | /* epsilon */ ;
declaration_var: VAR ',' declaration_var | VAR ;

return_stmt: RETURN expression ';' ;

RETURN : 'return' ;
CONST : '-'?[0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
VAR : [a-zA-Z][a-zA-Z0-9_]* ;
OPMULTDIV : ('*'|'/');
OPADDSUB : ('+'|'-');