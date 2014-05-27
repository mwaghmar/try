%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "acs_yacc.h"
#include "acs.h"

FILE* fp;
void print_token(const char* yytext)
{
#if 0
		fprintf(fp,"Token Start\n");
		fprintf(fp,"%s", yytext);
		fprintf(fp,"\nToken End\n");
#endif
}

%}

ID	[a-zA-Z_][a-zA-Z0-9\-_]*
CHALLENGE_BLOCK	[a-zA-Z0-9:]*
COMMA_CHAR [\,]
OB_CHAR [\(]
CB_CHAR [\)]
DOT_CHAR [\.]
COLONHIPHEN_CHARS ":\-"
OP "read"|"write"|"commit" 

%S PUB_KEY 
%S SIGNATURE
%S CLAUSE_START
%S VALIDITY
%S CHALLENGE
%S REQUEST_START
%S REPLY_START

%%

"-----BEGIN MPKI CREDENTIAL-----"   {
										print_token(yytext);
                                    	return CREDENTIALHEADER;    
                                    }

"-----END MPKI CREDENTIAL-----"     {
										print_token(yytext);
                                    	return CREDENTIALEND;
                                    }

"-----BEGIN PUBLIC KEY-----"       {	BEGIN PUB_KEY;   
										print_token(yytext);
                                    	return PUBLICKEYHEADER;
                                   }

"-----END PUBLIC KEY-----"         {	BEGIN 0;
										print_token(yytext);
                                    	return PUBLICKEYEND;
                                   }

"-----BEGIN SIGNATURE-----"        {	BEGIN SIGNATURE;
										print_token(yytext);
                                    	return SIGNATUREHEADER;
                                   }

"-----END SIGNATURE-----"          {	BEGIN 0;
										print_token(yytext);
                                   		return SIGNATUREEND;
                                   } 

"-----BEGIN VALIDITY-----"         {	BEGIN 0; BEGIN VALIDITY;
										print_token(yytext);
                                   		return VALIDITYHEADER;
                                   }

"-----END VALIDITY-----"           {    BEGIN 0; 
										print_token(yytext);
                                   		return VALIDITYEND;			
                                   }			

"-----BEGIN MPKI CERTIFICATE-----" {	BEGIN CLAUSE_START;   
										print_token(yytext);
								   		return CERTHEADER;
								   }

"-----END MPKI CERTIFICATE-----"   {
										print_token(yytext);
								   		return CERTEND;
								   }

"-----BEGIN MPKI CHALLENGE-----"   {
										BEGIN CHALLENGE;
										print_token(yytext);
								   		return CHALLENGEHEADER;
								   }

"-----END MPKI CHALLENGE-----"     {    BEGIN 0;
										print_token(yytext);
								   		return CHALLENGEEND;
								   }

"-----BEGIN MPKI CHALLENGE RESPONSE-----" {
												print_token(yytext);
                                          		return RESPONSEHEADER;	
                                          }

"-----END MPKI CHALLENGE RESPONSE-----"   {
												print_token(yytext);
                                          		return RESPONSEEND;
                                          }

"-----BEGIN REPOSITORY CLIENT REQUEST-----" {   BEGIN REQUEST_START;
												print_token(yytext);
                                          		return REQUESTHEADER;
                                            }

"-----END REPOSITORY CLIENT REQUEST-----"   {   BEGIN 0;
												print_token(yytext);
                                          		return REQUESTEND;
                                            }

"-----BEGIN REPOSITORY SERVER REPLY-----"   {	BEGIN REPLY_START;
												print_token(yytext);
                                          		return REPLYHEADER;
                                            }

"-----END REPOSITORY SERVER REPLY-----"   {		BEGIN 0;	  	
												print_token(yytext);
                                          		return REPLYEND;
                                          }

<VALIDITY>"Not before:"                   {
												print_token(yytext);
                                          		return NOTBEFORE;
                                          }

<VALIDITY>"datime"                        {
												strcpy(yylval.str, yytext);
												print_token(yytext);
                                          		return DATIME;
                                          }

<VALIDITY>"Not after:"                    {
												print_token(yytext);
                                          		return NOTAFTER;
                                          }

<PUB_KEY,SIGNATURE>([a-zA-Z0-9/+=]+[\n]?)+  {     
												strcpy(yylval.base64_block, yytext);	
												print_token(yytext);
                                          		return BASE64BLOCK;
                                         	 }

<CLAUSE_START>{ID}       					{
												if(!strcmp(yytext, "says")) {
													strcpy(yylval.str, yytext);
													return SAYS;
												}

												strcpy(yylval.str, yytext);
												print_token(yytext);
                                          		return ID;
									  		}


<VALIDITY>[0-9]*                          {
												yylval.num = atoi(yytext);
												print_token(yytext);
                                          		return NUM;
                                          }

<CHALLENGE>{CHALLENGE_BLOCK}			  {	
                                          		print_token(yytext);   
                                          		return CHALLENGE_BLK;
                                          }

{COMMA_CHAR}							  { yylval.ch = ','; return COMMA; }

{OB_CHAR}							  	  { return OB; }

{CB_CHAR}							      { return CB; }

<CLAUSE_START>{COLONHIPHEN_CHARS}		  { strcpy(yylval.str, yytext); return COLONHIPHEN; }
<CLAUSE_START,VALIDITY>{DOT_CHAR}		  { return DOT; }
<REQUEST_START>"request"                  { return REQUEST; }
<REQUEST_START>{OP}                  	  { return OPERATION; }
<REQUEST_START>"granted"|"denied"         { return RESULT; }

%%

int yywrap(void) {
    return 1;
}

/*
//#define LEX_HAS_MAIN 1
#ifdef LEX_HAS_MAIN
int main(int argc, char *argv[])
{
    ++argv, --argc;  // Skip over program name. 
    if (argc > 0)
        yyin = fopen(argv[0], "r");
    else
        yyin = stdin;

	fp = fopen("Tokens.txt", "w+");
	if( NULL == fp ) {
		printf("Failed to open Tokens.txt for writing");
	}

	while( CREDENTIALEND != yylex() );
}
#endif */
