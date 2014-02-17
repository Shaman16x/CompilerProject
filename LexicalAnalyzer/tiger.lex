%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

int charPos=1;
static int comment_level = 0;

int yywrap(void)
{
    if(comment_level){
    	EM_error(EM_tokPos,"A comment was not closed with */");
    }
    comment_level = 0;
    charPos=1;
    return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}


%}

INT     [0-9]+
ID		  [a-zA-Z][a-z0-9A-Z_]*
STRING  "\""([^"\n]|("\\\""))*"\""

%%



" "         {adjust(); continue;}
\t          {adjust(); continue;}
\n          {adjust(); EM_newline(); continue;}
\r          {adjust(); EM_newline(); continue;}
","         {adjust(); if (comment_level) continue; else return COMMA;}
":"         {adjust(); if (comment_level) continue; else return COLON;}
";"         {adjust(); if (comment_level) continue; else return SEMICOLON;}
"("         {adjust(); if (comment_level) continue; else return LPAREN;}
"["         {adjust(); if (comment_level) continue; else  return LBRACK;}
")"         {adjust(); if (comment_level) continue; else return RPAREN;}
"]"         {adjust(); if (comment_level) continue; else return RBRACK;}
"{"         {adjust(); if (comment_level) continue; else return LBRACE;}
"}"         {adjust(); if (comment_level) continue; else return RBRACE;}
"."         {adjust(); if (comment_level) continue; else return DOT;}
"+"         {adjust(); if (comment_level) continue; else return PLUS;}
"-"         {adjust(); if (comment_level) continue; else return MINUS;}
"*"         {adjust(); if (comment_level) continue; else return TIMES;}
"/"         {adjust(); if (comment_level) continue; else return DIVIDE;}
"="         {adjust(); if (comment_level) continue; else return EQ;}
"!="        {adjust(); if (comment_level) continue; else return NEQ;}
"<"         {adjust(); if (comment_level) continue; else  return LT;}
"<="        {adjust(); if (comment_level) continue; else return LE;}
">"         {adjust(); if (comment_level) continue; else return GT;}
">="        {adjust(); if (comment_level) continue; else return GE;}
"&"         {adjust(); if (comment_level) continue; else return AND;}
"|"	      {adjust(); if (comment_level) continue; else return OR;}
":="	   	{adjust(); if (comment_level) continue; else return ASSIGN;}
"array"	   {adjust(); if (comment_level) continue; else return ARRAY;}
if          {adjust(); if (comment_level) continue; else return IF;}
then        {adjust(); if (comment_level) continue; else return THEN;}
else        {adjust(); if (comment_level) continue; else return ELSE;}
while       {adjust(); if (comment_level) continue; else return WHILE;}
for         {adjust(); if (comment_level) continue; else return FOR;}
to          {adjust(); if (comment_level) continue; else return TO;}
do          {adjust(); if (comment_level) continue; else return DO;}
let         {adjust(); if (comment_level) continue; else return LET;}
in          {adjust(); if (comment_level) continue; else return IN;}
end         {adjust(); if (comment_level) continue; else return END;}
of          {adjust(); if (comment_level) continue; else return OF;}
break       {adjust(); if (comment_level) continue; else return BREAK;}
nil         {adjust(); if (comment_level) continue; else return NIL;}
function    {adjust(); if (comment_level) continue; else return FUNCTION;}
var         {adjust(); if (comment_level) continue; else return VAR;}
type        {adjust(); if (comment_level) continue; else return TYPE;}


{INT}       {adjust(); if (comment_level) continue; else yylval.ival=atoi(yytext); return INT;}
{ID}		{adjust(); if (comment_level) continue; else yylval.sval=String(yytext); return ID;}
{STRING}    {adjust(); yylval.sval=yytext; return STRING;}

"/*"	{
	adjust();
	comment_level++;
	continue;
}

"*/" {
    adjust();
    if(comment_level > 0) {comment_level--;}
    else {EM_error(EM_tokPos,"Close comment symbol does not have a matching open comment symbol");}
    continue;
}

"\"" {adjust(); if (comment_level) continue; else EM_error(EM_tokPos,"String Literal does not have closing \" on same line");}
.	 {adjust(); if (comment_level) continue; else EM_error(EM_tokPos,"illegal token");}

