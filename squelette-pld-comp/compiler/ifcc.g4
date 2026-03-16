grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' block;

block
    : '{' stmt* '}'
    ;

stmt 
    : declaration
    | declaration_var
    | return_stmt
    | affectation_declaration
    | block
    | expression ';'
    | conditional
    ;

conditional
    : 'if' '(' expression ')' block else_stmt*
    ;

else_stmt
    : 'else' 'if' '(' expression ')' block    #else_if
    | 'else' block                     #else
    ;


declaration : 'int' declaration_var ';' ;
declaration_var : VAR ',' declaration_var | VAR ;
return_stmt : 'return' expression ';' ;
affectation_declaration: 'int' VAR '=' expression ';' ;


expression 	: '(' expression ')'                            #par
			| NOT expression							    #not
           	| MINUS expression                              #opposite
           	| expression op=(MUL|DIV|MOD) expression        #muldiv
           	| expression op=(PLUS|MINUS) expression         #addsub
           	| expression op=(LTE|LT|GTE|GT) expression  	#comp
            | expression op=(EQ|NEQ) expression          	#eq
            | expression BITWISE_AND expression				#bitwise_and
            | expression BITWISE_XOR expression				#bitwise_xor
            | expression BITWISE_OR expression				#bitwise_or 
           	| VAR '=' expression						    #affectation
            | PUTCHAR '(' expression ')'                    #putchar
            | GETCHAR '(' ')'                               #getchar
           	| CONST                                         #const
            | CHARCONST                                     #charconst
           	| VAR                                           #var
            ;


CHARCONST
    : '\'' ( ~['\\\r\n] | EscapeSequence )+ '\'' ;

fragment EscapeSequence
    : '\\' [nrt'\\]
    | '\\x' [0-9a-fA-F]+
    | '\\' [0-7]+
    ;

CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
PUTCHAR : 'putchar';
GETCHAR : 'getchar';
VAR : [a-zA-Z][a-zA-Z0-9_]* ;
PLUS : '+';
MINUS : '-';
MUL : '*';
DIV : '/';
MOD : '%';
EQ : '==';
NEQ : '!=';
GT : '>';
GTE : '>=';
LT : '<';
LTE : '<=';
NOT : '!';
BITWISE_OR : '|';
BITWISE_AND : '&';
BITWISE_XOR : '^';
