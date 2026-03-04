grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' declaration bloc return_stmt '}' ;

bloc : affectation bloc | /*epsilon*/ ;

declaration : 'int' declaration_var ';' | /*epsilon*/ ;
declaration_var : VAR ',' declaration_var | VAR ;
affectation : VAR '=' expression ';' ;

expression : expression OPMULDIV expression	    #muldiv
	| expression OPADDSUB expression	        #addsub
	| '(' expression ')'			            #par
	| CONST 				                    #const
	| VAR 					                    #var;

return_stmt : RETURN expression ';' ;

RETURN : 'return' ;
CONST : '-'?[0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
VAR : [a-zA-Z][a-zA-Z0-9_]* ;
OPMULDIV : ('*'|'/') ;
OPADDSUB : ('+'|'-') ;
