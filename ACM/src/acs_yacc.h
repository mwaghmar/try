#include "acs.h"
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
extern YYSTYPE yylval;
