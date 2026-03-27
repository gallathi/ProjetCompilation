grammar ifcc;

axiom : prog EOF ;

prog : function_def+;

function_def : type VAR '(' param_list? ')' block;

param_list : param (',' param)*;

param : type VAR;

type : 'int' | 'void';

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
    | switch_stmt
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
declaration_var : declaration_item (',' declaration_item)* ;
declaration_item : VAR (AEQ expression)? ;
return_stmt : 'return' expression? ';' ;
affectation_declaration: 'int' VAR '=' expression ';' ;
arg_list : expression (',' expression)* ;
switch_stmt : 'switch' '(' expression ')' '{' switch_case* switch_default? '}' ;
switch_case : 'case' CONST ':' stmt* ;
switch_default : 'default' ':' stmt* ;

expression 	: '(' expression ')'                            #par
			| NOT expression							    #not
			| VAR INCR										#post_incr
			| VAR DECR										#post_decr
			| INCR VAR										#pre_incr
			| DECR VAR                        				#pre_decr
           		| MINUS expression                              #opposite
           		| expression op=(MUL|DIV|MOD) expression        #muldiv
           		| expression op=(PLUS|MINUS) expression         #addsub
           	| expression op=(LTE|LT|GTE|GT) expression  	#comp
            | expression op=(EQ|NEQ) expression          	#eq
            | expression BITWISE_AND expression				#bitwise_and
            | expression BITWISE_XOR expression				#bitwise_xor
            | expression BITWISE_OR expression				#bitwise_or 
            | expression LOGICAL_AND expression            #logical_and
            | expression LOGICAL_OR expression             #logical_or
           	| VAR op=(AEQ|PEQ|MEQ) expression				#affectation
            | VAR '(' arg_list? ')'                         #call
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
INCR : '++';
DECR : '--';
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
LOGICAL_AND : '&&';
LOGICAL_OR : '||';
AEQ : '=';
PEQ : '+=';
MEQ : '-=';
CONTINUE : 'continue';
BREAK : 'break';
CONST : [0-9]+ ;
VAR : [a-zA-Z][a-zA-Z0-9_]* ;
