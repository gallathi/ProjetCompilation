grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' block;

block
    : '{' stmt* '}'
    ;

stmt 
    : declaration
    | CONTINUE ';'
    | BREAK ';'
    | return_stmt
    | block
    | expression ';'
    | conditional
    | while_conditional
    ;

conditional
    : 'if' '(' expression ')' block else_stmt*
    ;

while_conditional
    : 'while' '(' expression ')' block
    ;

else_stmt
    : 'else' 'if' '(' expression ')' block    #else_if
    | 'else' block                     #else
    ;


declaration : 'int' declaration_var ';' ;
declaration_var : (VAR|decla_affect) ',' declaration_var | (VAR|decla_affect) ;
return_stmt : 'return' expression ';' ;


expression 	: '(' expression ')'                            #par
			| NOT expression							    #not
			| VAR PLUS PLUS									#post_incr
			| VAR MINUS MINUS								#post_decr
			| PLUS PLUS VAR									#pre_incr
			| MINUS MINUS VAR                        		#pre_decr
           	| MINUS expression                              #opposite
           	| expression op=(MUL|DIV|MOD) expression        #muldiv
           	| expression op=(PLUS|MINUS) expression         #addsub
           	| expression op=(LTE|LT|GTE|GT) expression  	#comp
            | expression op=(EQ|NEQ) expression          	#eq
            | expression BITWISE_AND expression				#bitwise_and
            | expression BITWISE_XOR expression				#bitwise_xor
            | expression BITWISE_OR expression				#bitwise_or 
           	| VAR op=(AEQ|PEQ|MEQ) expression				#affectation
            | PUTCHAR '(' expression ')'                    #putchar
            | GETCHAR '(' ')'                               #getchar
           	| CONST                                         #const
            | CHARCONST                                     #charconst
           	| VAR                                           #var
            ;

decla_affect : VAR AEQ expression ;

CHARCONST
    : '\'' ( ~['\\\r\n] | EscapeSequence )+ '\'' ;

fragment EscapeSequence
    : '\\' [nrt'\\]
    | '\\x' [0-9a-fA-F]+
    | '\\' [0-7]+
    ;

COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
PUTCHAR : 'putchar';
GETCHAR : 'getchar';
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
AEQ : '=';
PEQ : '+=';
MEQ : '-=';
CONTINUE : 'continue';
BREAK : 'break';
CONST : [0-9]+ ;
VAR : [a-zA-Z][a-zA-Z0-9_]* ;
