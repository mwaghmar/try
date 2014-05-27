#ifndef lint
static const char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif

#include <stdlib.h>
#include <string.h>

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20070509

#define YYEMPTY (-1)
#define yyclearin    (yychar = YYEMPTY)
#define yyerrok      (yyerrflag = 0)
#define YYRECOVERING (yyerrflag != 0)

extern int yyparse(void);

static int yygrowstack(void);
#define YYPREFIX "yy"
#line 2 "acs.y"
#include <stdio.h>
#include "acs.h"

#define MAX_BASE64_BLK_LENGTH	2048
void yyerror(char*);
int yylex();
extern FILE* yyin;
#line 11 "acs.y"
typedef union{
char				ch;
int					num;
char				str[100];
char 				base64_block[2048];  /*MAX_BASE64_BLK_LENGTH*/
char				literal[2048];  /*MAX_LITERAL_LENGTH*/
char				predicate[1024]; /*MAX_PREDICATE_LENGTH*/
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
} YYSTYPE;
#line 56 "acs_yacc.c"
#define CREDENTIALHEADER 257
#define CREDENTIALEND 258
#define PUBLICKEYHEADER 259
#define PUBLICKEYEND 260
#define SIGNATUREHEADER 261
#define SIGNATUREEND 262
#define VALIDITYHEADER 263
#define VALIDITYEND 264
#define CERTHEADER 265
#define CERTEND 266
#define CHALLENGEHEADER 267
#define CHALLENGEEND 268
#define RESPONSEHEADER 269
#define RESPONSEEND 270
#define REQUESTHEADER 271
#define REQUESTEND 272
#define REPLYHEADER 273
#define REPLYEND 274
#define NOTBEFORE 275
#define NOTAFTER 276
#define BASE64BLOCK 277
#define NUM 278
#define ID 279
#define CHALLENGE_BLK 280
#define COMMA 281
#define OB 282
#define CB 283
#define COLONHIPHEN 284
#define DOT 285
#define SAYS 286
#define DATIME 287
#define REQUEST 288
#define OPERATION 289
#define RESULT 290
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    1,    2,    2,    2,    3,    5,    4,    4,    6,
    7,    8,    8,    9,    9,   10,   10,   11,   11,   12,
   13,   13,   14,   15,   16,   17,   18,   19,
};
short yylen[] = {                                         2,
    4,    3,    1,    2,    0,    6,    3,    1,    0,    6,
   15,    1,    2,    2,    4,    1,    3,    1,    6,    4,
    1,    3,    3,    3,    8,    1,    3,    1,
};
short yydefred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    3,    2,    0,
    0,    0,   12,    0,   18,    1,    4,    0,    0,    0,
    0,    8,   13,    0,   14,   21,    0,    0,    0,    0,
    0,   16,    0,   20,    0,    0,    0,    0,    0,    0,
   15,   22,    0,    0,    0,    0,    6,   17,   19,    0,
    0,    7,    0,   10,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   11,
};
short yydgoto[] = {                                       2,
    4,    7,    8,   21,   39,   22,   37,   12,   13,   31,
   14,   15,   27,    0,    0,    0,    0,    0,    0,
};
short yysindex[] = {                                   -245,
 -246,    0, -262, -251, -242, -277, -255,    0,    0, -261,
 -260, -263,    0, -280,    0,    0,    0, -259, -254, -256,
 -246,    0,    0, -277,    0,    0, -275, -257, -258, -235,
 -274,    0, -252,    0, -249, -250, -248, -244, -232, -277,
    0,    0, -247, -243, -258, -231,    0,    0,    0, -241,
 -227,    0, -240,    0, -239, -237, -238, -234, -236, -230,
 -229, -228, -226, -225,    0,
};
short yyrindex[] = {                                      0,
    0,    0,    0, -219,    0,    0,    0,    0,    0,    0,
    0, -213,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,
};
short yygindex[] = {                                      0,
   26,    0,   42,    0,    0,    0,    6,    0,   41,    0,
  -23,   19,    0,    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 60
short yytable[] = {                                      20,
   32,   10,   16,   24,   25,   33,   40,   34,   11,    6,
   41,    1,    3,    6,    5,   10,   48,    9,   29,   26,
   18,   19,   11,   35,   28,   38,   42,   45,   36,   10,
   52,   44,   46,   47,   50,   49,   54,   55,    5,   53,
   57,   56,   58,   59,   60,    9,   30,   61,   17,   63,
   51,   62,   23,   43,    0,    0,   64,    0,    0,   65,
};
short yycheck[] = {                                     263,
   24,  279,  258,  284,  285,  281,  281,  283,  286,  265,
  285,  257,  259,  265,  277,  279,   40,  260,  275,  279,
  282,  282,  286,  281,  279,  261,  279,  276,  287,  279,
  262,  282,  277,  266,  278,  283,  264,  278,  258,  281,
  278,  281,  281,  278,  281,  259,   21,  278,    7,  278,
   45,  281,   12,   35,   -1,   -1,  283,   -1,   -1,  285,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 290
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"CREDENTIALHEADER",
"CREDENTIALEND","PUBLICKEYHEADER","PUBLICKEYEND","SIGNATUREHEADER",
"SIGNATUREEND","VALIDITYHEADER","VALIDITYEND","CERTHEADER","CERTEND",
"CHALLENGEHEADER","CHALLENGEEND","RESPONSEHEADER","RESPONSEEND","REQUESTHEADER",
"REQUESTEND","REPLYHEADER","REPLYEND","NOTBEFORE","NOTAFTER","BASE64BLOCK",
"NUM","ID","CHALLENGE_BLK","COMMA","OB","CB","COLONHIPHEN","DOT","SAYS",
"DATIME","REQUEST","OPERATION","RESULT",
};
char *yyrule[] = {
"$accept : Credential",
"Credential : CREDENTIALHEADER PublicKey CertificateList CREDENTIALEND",
"PublicKey : PUBLICKEYHEADER BASE64BLOCK PUBLICKEYEND",
"CertificateList : Certificate",
"CertificateList : CertificateList Certificate",
"CertificateList :",
"Certificate : CERTHEADER ClauseList ValidityList PublicKey Signature CERTEND",
"Signature : SIGNATUREHEADER BASE64BLOCK SIGNATUREEND",
"ValidityList : Validity",
"ValidityList :",
"Validity : VALIDITYHEADER NOTBEFORE TimeTuple NOTAFTER TimeTuple VALIDITYEND",
"TimeTuple : DATIME OB NUM COMMA NUM COMMA NUM COMMA NUM COMMA NUM COMMA NUM CB DOT",
"ClauseList : Clause",
"ClauseList : ClauseList Clause",
"Clause : Literal DOT",
"Clause : Literal COLONHIPHEN LiteralList DOT",
"LiteralList : Literal",
"LiteralList : LiteralList COMMA Literal",
"Literal : Predicate",
"Literal : SAYS OB ID COMMA Predicate CB",
"Predicate : ID OB IDList CB",
"IDList : ID",
"IDList : IDList COMMA ID",
"Challenge : CHALLENGEHEADER CHALLENGE_BLK CHALLENGEEND",
"Response : RESPONSEHEADER BASE64BLOCK RESPONSEEND",
"Request : REQUESTHEADER REQUEST OB Op COMMA ID CB REQUESTEND",
"Op : OPERATION",
"Reply : REPLYHEADER Result REPLYEND",
"Result : RESULT",
};
#endif
#if YYDEBUG
#include <stdio.h>
#endif

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH  500
#endif
#endif

#define YYINITSTACKSIZE 500

int      yydebug;
int      yynerrs;
int      yyerrflag;
int      yychar;
short   *yyssp;
YYSTYPE *yyvsp;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* variables for the parser stack */
static short   *yyss;
static short   *yysslim;
static YYSTYPE *yyvs;
static int      yystacksize;
#line 307 "acs.y"

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
#line 332 "acs_yacc.c"
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(void)
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = yyssp - yyss;
    newss = (yyss != 0)
          ? (short *)realloc(yyss, newsize * sizeof(*newss))
          : (short *)malloc(newsize * sizeof(*newss));
    if (newss == 0)
        return -1;

    yyss  = newss;
    yyssp = newss + i;
    newvs = (yyvs != 0)
          ? (YYSTYPE *)realloc(yyvs, newsize * sizeof(*newvs))
          : (YYSTYPE *)malloc(newsize * sizeof(*newvs));
    if (newvs == 0)
        return -1;

    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse(void)
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    yyerror("syntax error");

#ifdef lint
    goto yyerrlab;
#endif

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yyvsp[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 1:
#line 91 "acs.y"
{
				Credential_t* pCred;
				pCred = (Credential_t*) malloc(sizeof(Credential_t));	
				pCred->public_key = yyvsp[-2].pk; 
				pCred->cert_list = yyvsp[-1].cert_ptr; 
				yyval.cred = pCred;
				gpCredential = yyval.cred;
			}
break;
case 2:
#line 102 "acs.y"
{
				PublicKey_t p;
				memset(&p, 0, sizeof(p));
				strcpy(p.base64_pk, yyvsp[-1].base64_block);
				/*fix_newline_char(p.base64_pk);*/
				yyval.pk = p;
			}
break;
case 3:
#line 112 "acs.y"
{
					yyval.cert_ptr = getCertificateNode(yyvsp[0].cert);
			}
break;
case 4:
#line 116 "acs.y"
{
				CertificateNode_t itr = yyval.cert_ptr; 
				CertificateNode_t node = getCertificateNode(yyvsp[0].cert);
				APPEND_NODE(itr, node);
			}
break;
case 5:
#line 121 "acs.y"
{ yyval.cert_ptr = NULL; }
break;
case 6:
#line 125 "acs.y"
{
				Certificate_t c;
				memset(&c, 0, sizeof(c));	
				c.clause_list = yyvsp[-4].cl_ptr;
				c.validity_list = yyvsp[-3].validity_ptr;
				c.public_key = yyvsp[-2].pk;
				c.signature = yyvsp[-1].sign;
				yyval.cert = c;
			}
break;
case 7:
#line 137 "acs.y"
{
				Signature_t s;
				memset(&s, 0, sizeof(Signature_t));
				strcpy(s.sign, yyvsp[-1].base64_block);
				/*fix_newline_char(s.sign);*/
				yyval.sign = s;
			}
break;
case 8:
#line 148 "acs.y"
{
				Validity_t* vn = (Validity_t*) malloc(sizeof(Validity_t));
				*vn = yyvsp[0].validity;
				yyval.validity_ptr = vn;
			}
break;
case 9:
#line 153 "acs.y"
{
				yyval.validity_ptr = NULL;
			}
break;
case 10:
#line 159 "acs.y"
{
			Validity_t vt;
			vt.not_before = yyvsp[-3].time_tuple;
			vt.not_after = yyvsp[-1].time_tuple; 
			yyval.validity = vt;
		}
break;
case 11:
#line 168 "acs.y"
{
				TimeTuple_t tt;
				tt.year = yyvsp[-12].num;
				tt.month = yyvsp[-10].num;
				tt.day = yyvsp[-8].num;
				tt.hour = yyvsp[-6].num;
				tt.min = yyvsp[-4].num;
				tt.sec = yyvsp[-2].num;
				yyval.time_tuple = tt;
			}
break;
case 12:
#line 181 "acs.y"
{
					yyval.cl_ptr = getClauseNode(yyvsp[0].cl);
			}
break;
case 13:
#line 185 "acs.y"
{
				ClauseNode_t itr = yyval.cl_ptr; 
				ClauseNode_t node = getClauseNode(yyvsp[0].cl);
				APPEND_NODE(itr, node);
			}
break;
case 14:
#line 193 "acs.y"
{
				Clause_t tmp;
				memset(&tmp, 0, sizeof(tmp));
				strcpy(tmp.clause, yyvsp[-1].literal);
				strcat(tmp.clause, ".");
				yyval.cl = tmp;
			}
break;
case 15:
#line 201 "acs.y"
{
				struct LiteralNode* itr = yyvsp[-1].literal_list;
				unsigned LiteralNodeCount = 1;

				Clause_t tmp;
				memset(&tmp, 0, sizeof(tmp));
				strcpy(tmp.clause, yyvsp[-3].literal);
				strcat(tmp.clause, yyvsp[-2].str);

				while(itr) {
					if( LiteralNodeCount++ > 1) {
						strcat(tmp.clause, ",");
					}
					strcat(tmp.clause, itr->literal);
					itr = itr->next;
				}
				strcat(tmp.clause, ".");
				yyval.cl = tmp;
			}
break;
case 16:
#line 223 "acs.y"
{
					yyval.literal_list = getLiteralNode(yyvsp[0].literal);
			}
break;
case 17:
#line 227 "acs.y"
{
				struct LiteralNode* itr = yyval.literal_list; 
				struct LiteralNode* node = getLiteralNode(yyvsp[0].literal);
				APPEND_NODE(itr, node);
			}
break;
case 18:
#line 235 "acs.y"
{
			memset(yyval.literal, 0, sizeof(yyval.literal));
			strcat(yyval.literal, yyvsp[0].predicate);
		}
break;
case 19:
#line 240 "acs.y"
{
			memset(yyval.literal, 0, sizeof(yyval.literal));
			strcat(yyval.literal, yyvsp[-5].str);
			strcat(yyval.literal, "(");
			strcat(yyval.literal, yyvsp[-3].str);
			strcat(yyval.literal, ",");
			strcat(yyval.literal, yyvsp[-1].predicate);
			strcat(yyval.literal, ")");
		}
break;
case 20:
#line 252 "acs.y"
{
				struct IDNode* itr = yyvsp[-1].id_list;
				int IdNodeCount = 1;

				char* s = yyval.predicate;
				memset(s, 0, MAX_PREDICATE_LENGTH);
				strcpy(yyval.predicate, yyvsp[-3].str);
				strcat(yyval.predicate, "(");

				while(itr) {
					if( IdNodeCount++ > 1) {
						strcat(yyval.predicate, ",");
					}
					strcat(yyval.predicate, itr->id);
					itr = itr->next;
				}

				strcat(yyval.predicate, ")");
			}
break;
case 21:
#line 273 "acs.y"
{  
			yyval.id_list = getIDNode(yyvsp[0].str);
         }
break;
case 22:
#line 277 "acs.y"
{
			struct IDNode* itr = yyval.id_list; 
			struct IDNode* node = getIDNode(yyvsp[0].str);
			APPEND_NODE(itr, node);
		}
break;
#line 725 "acs_yacc.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;

yyoverflow:
    yyerror("yacc stack overflow");

yyabort:
    return (1);

yyaccept:
    return (0);
}
