grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' block ;

block
    : '{' stmt* '}'
    ;

stmt 
    : declaration
    | declaration_var
    | affectation 
    | return_stmt 
    ; 



declaration : 'int' declaration_var ';' | /*epsilon*/ ;
declaration_var : VAR ',' declaration_var | VAR ;
affectation : VAR '=' expression ';' ;
return_stmt : RETURN expression ';' ;


expression : '(' expression ')'                         #par
           | MINUS expression                           #opposite
           | expression op=(MUL|DIV|MOD) expression     #muldiv
           | expression op=(PLUS|MINUS) expression      #addsub
           | CONST                                      #const
           | VAR                                        #var;



RETURN : 'return' ;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
VAR : [a-zA-Z][a-zA-Z0-9_]* ;
PLUS : '+';
MINUS : '-';
MUL : '*';
DIV : '/';
MOD : '%';
