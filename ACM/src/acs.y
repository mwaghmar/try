%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "acs.h"

#define MAX_BASE64_BLK_LENGTH	2048
void yyerror(char*);
int yylex();
extern FILE* yyin;
%}

%union{
char				ch;
int					num;
char				str[100];
char 				base64_block[2048];  //MAX_BASE64_BLK_LENGTH
char				literal[2048];  //MAX_LITERAL_LENGTH
char				predicate[1024]; //MAX_PREDICATE_LENGTH
Clause_t 			cl;
ClauseNode_t    	cl_ptr;

PublicKey_t 		pk;
Signature_t			sign;
struct IDNode*      id_list;
struct LiteralNode* literal_list;

TimeTuple_t     	time_tuple;
Validity_t			validity;
Validity_t*			validity_ptr;

Certificate_t		cert;
CertificateNode_t	cert_ptr;

Credential_t*		cred;
}

%token CREDENTIALHEADER
%token CREDENTIALEND 
%token PUBLICKEYHEADER
%token PUBLICKEYEND 
%token SIGNATUREHEADER
%token SIGNATUREEND
%token VALIDITYHEADER
%token VALIDITYEND 
%token CERTHEADER
%token CERTEND
%token CHALLENGEHEADER
%token CHALLENGEEND
%token RESPONSEHEADER 
%token RESPONSEEND
%token REQUESTHEADER
%token REQUESTEND
%token REPLYHEADER
%token REPLYEND	
%token NOTBEFORE
%token NOTAFTER
%token <base64_block> BASE64BLOCK
%token <num> NUM
%token <str> ID
%token CHALLENGE_BLK
%token <ch> COMMA
%token <ch> OB
%token <ch> CB
%token <str> COLONHIPHEN
%token <ch> DOT
%token <str> SAYS
%token <str> DATIME
%token REQUEST
%token OPERATION
%token RESULT

%type <pk> PublicKey	
%type <cert_ptr> CertificateList
%type <cert> Certificate
%type <validity_ptr> ValidityList
%type <sign> Signature
%type <validity> Validity
%type <time_tuple> TimeTuple
%type <cl_ptr> ClauseList
%type <cl> Clause
%type <literal_list> LiteralList
%type <literal> Literal
%type <predicate> Predicate
%type <id_list> IDList
%type <cred> Credential

%start Credential
%%

Credential: 
		  CREDENTIALHEADER PublicKey CertificateList CREDENTIALEND
			{
				Credential_t* pCred;
				pCred = (Credential_t*) malloc(sizeof(Credential_t));	
				pCred->public_key = $2; 
				pCred->cert_list = $3; 
				$$ = pCred;
				gpCredential = $$;
			}

PublicKey:
		 PUBLICKEYHEADER BASE64BLOCK PUBLICKEYEND
			{
				PublicKey_t p;
				memset(&p, 0, sizeof(p));
				strcpy(p.base64_pk, $2);
				//fix_newline_char(p.base64_pk);
				$$ = p;
			}
		 
CertificateList: 
			Certificate 
			{
					$$ = getCertificateNode($1);
			} 
		|	CertificateList Certificate
			{
				CertificateNode_t itr = $$; 
				CertificateNode_t node = getCertificateNode($2);
				APPEND_NODE(itr, node);
			}
		| { $$ = NULL; }

Certificate:
		   CERTHEADER ClauseList ValidityList PublicKey Signature CERTEND
		   	{
				Certificate_t c;
				memset(&c, 0, sizeof(c));	
				c.clause_list = $2;
				c.validity_list = $3;
				c.public_key = $4;
				c.signature = $5;
				$$ = c;
			}

Signature:
		 SIGNATUREHEADER BASE64BLOCK SIGNATUREEND
			{
				Signature_t s;
				memset(&s, 0, sizeof(Signature_t));
				strcpy(s.sign, $2);
				//fix_newline_char(s.sign);
				$$ = s;
			}
		

ValidityList:
		   	Validity
			{
				Validity_t* vn = (Validity_t*) malloc(sizeof(Validity_t));
				*vn = $1;
				$$ = vn;
			}		
		|	{
				$$ = NULL;
			}

Validity: 
		VALIDITYHEADER NOTBEFORE TimeTuple NOTAFTER TimeTuple VALIDITYEND
		{
			Validity_t vt;
			vt.not_before = $3;
			vt.not_after = $5; 
			$$ = vt;
		}

TimeTuple:
		 DATIME OB NUM COMMA NUM COMMA NUM COMMA NUM COMMA NUM COMMA NUM CB DOT
			{
				TimeTuple_t tt;
				tt.year = $3;
				tt.month = $5;
				tt.day = $7;
				tt.hour = $9;
				tt.min = $11;
				tt.sec = $13;
				$$ = tt;
			}
		
ClauseList:
		  Clause
			{
					$$ = getClauseNode($1);
			} 
		| ClauseList Clause
			{
				ClauseNode_t itr = $$; 
				ClauseNode_t node = getClauseNode($2);
				APPEND_NODE(itr, node);
			} 

Clause:
	  	Literal DOT 
			{
				Clause_t tmp;
				memset(&tmp, 0, sizeof(tmp));
				strcpy(tmp.clause, $1);
				strcat(tmp.clause, ".");
				$$ = tmp;
			}	
		| Literal COLONHIPHEN LiteralList DOT
			{
				struct LiteralNode* itr = $3;
				unsigned LiteralNodeCount = 1;

				Clause_t tmp;
				memset(&tmp, 0, sizeof(tmp));
				strcpy(tmp.clause, $1);
				strcat(tmp.clause, $2);

				while(itr) {
					if( LiteralNodeCount++ > 1) {
						strcat(tmp.clause, ",");
					}
					strcat(tmp.clause, itr->literal);
					itr = itr->next;
				}
				strcat(tmp.clause, ".");
				$$ = tmp;
			}	

LiteralList:
		  Literal
			{
					$$ = getLiteralNode($1);
			}
		| LiteralList COMMA Literal
			{
				struct LiteralNode* itr = $$; 
				struct LiteralNode* node = getLiteralNode($3);
				APPEND_NODE(itr, node);
			}

Literal: 
	   Predicate 
		{
			memset($$, 0, sizeof($$));
			strcat($$, $1);
		}
	| SAYS OB ID COMMA Predicate CB
		{
			memset($$, 0, sizeof($$));
			strcat($$, $1);
			strcat($$, "(");
			strcat($$, $3);
			strcat($$, ",");
			strcat($$, $5);
			strcat($$, ")");
		}

Predicate: 
		 ID OB IDList CB
			{
				struct IDNode* itr = $3;
				int IdNodeCount = 1;

				char* s = $$;
				memset(s, 0, MAX_PREDICATE_LENGTH);
				strcpy($$, $1);
				strcat($$, "(");

				while(itr) {
					if( IdNodeCount++ > 1) {
						strcat($$, ",");
					}
					strcat($$, itr->id);
					itr = itr->next;
				}

				strcat($$, ")");
			}

IDList: 
	  ID {  
			$$ = getIDNode($1);
         } 
	| IDList COMMA ID
		{
			struct IDNode* itr = $$; 
			struct IDNode* node = getIDNode($3);
			APPEND_NODE(itr, node);
		}	

Challenge:
		 CHALLENGEHEADER CHALLENGE_BLK CHALLENGEEND
	;

Response:
		RESPONSEHEADER BASE64BLOCK RESPONSEEND
	;

Request:
	   REQUESTHEADER REQUEST OB Op COMMA ID CB REQUESTEND
	;

Op:
  	OPERATION
	;

Reply:
	 REPLYHEADER Result REPLYEND
	;

Result:
	  RESULT
	;
%%

#define  APPEND_NODE(itr , node)  			\
			do {							\
				while(itr->next) {			\
					itr = itr->next;		\
				}							\
				itr->next = (node);			\
			}while(0)						

struct IDNode* getIDNode(const char* id) 
{
	struct IDNode* tmp = (struct IDNode*) malloc(sizeof(struct IDNode));
	tmp->next = NULL;
	memset(tmp->id,0, sizeof(tmp->id));
	strcpy(tmp->id, id);
	return tmp;
}

struct LiteralNode* getLiteralNode(const char* literal) 
{
	struct LiteralNode* tmp = (struct LiteralNode*) malloc(sizeof(struct LiteralNode));
	tmp->next = NULL;
	memset(tmp->literal,0, sizeof(tmp->literal));
	strcpy(tmp->literal, literal);
	return tmp;
}

ClauseNode_t getClauseNode(Clause_t cl) 
{
	ClauseNode_t tmp = (ClauseNode_t) malloc(sizeof(struct ClauseNode));
	tmp->next = NULL;
	tmp->cl = cl;
	return tmp;
}

CertificateNode_t getCertificateNode(Certificate_t cert) 
{
	CertificateNode_t tmp = (CertificateNode_t) malloc(sizeof(struct CertificateNode));
	tmp->cert = cert;
	tmp->next = NULL;
	return tmp;
}

void fix_newline_char(char* base64_blk)
{
	char* ptr =NULL;
	char* blkptr = base64_blk;

    while(ptr = strstr(blkptr, "\n")) {
		strcpy(ptr, ptr+1);
		blkptr = ptr;
	}
}

void yyerror(char* errstr)
{
	fprintf(stderr, "%s\n", errstr);
}

Credential_t* gpCredential;
Credential_t* parse_credential(const char* cred_file_name)
{
	Credential_t* pCred = NULL;

	yyin = fopen(cred_file_name, "r");
	if( NULL == yyin ) {
			return NULL;
	}

	if( 0 == yyparse() ) {
		return pCred = gpCredential;	
	}

	return NULL;
}

#if 0

int main(int argc, char* argv[])
{
	++argv; --argc;
    if (argc > 0)
        yyin = fopen(argv[0], "r");
    else
        yyin = stdin;

	yyparse();
	printf("\n%s", gpCredential->public_key.base64_pk);
	return 0;
}
#endif
