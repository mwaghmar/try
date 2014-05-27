#ifndef __ACS_H__
#define __ACS_H__

#define BASE64_PK_LENGTH 		512
#define PK_LENGTH 				1024
#define MAX_CLAUSE_LENGTH 		2560
#define MAX_SIGN_LENGTH 		2048

#define MAX_ID_LENGTH			64
#define MAX_LITERAL_LENGTH		2048
#define MAX_PREDICATE_LENGTH	1024

typedef struct Signature
{
		char 	sign[MAX_SIGN_LENGTH];
}Signature_t;

typedef struct PublicKey
{
		char 	base64_pk[BASE64_PK_LENGTH];
		char 	pk[PK_LENGTH];
}PublicKey_t;

typedef struct Clause
{
	   	char 	clause[MAX_CLAUSE_LENGTH];		
} Clause_t;

typedef struct ClauseNode
{
		Clause_t  				cl;
		struct ClauseNode* 		next;
}*ClauseNode_t;

typedef struct TimeTuple
{
		unsigned int year, month, day, hour, min, sec;
}TimeTuple_t;

typedef struct Validity
{
		TimeTuple_t 	not_before;
		TimeTuple_t 	not_after;
}Validity_t;

typedef struct Certificate
{
		ClauseNode_t 	clause_list;
		Validity_t*		validity_list;
		PublicKey_t 	public_key;
		Signature_t		signature;
}Certificate_t;

typedef struct CertificateNode
{
		Certificate_t				cert;
		struct CertificateNode* 	next;
}*CertificateNode_t;

struct IDNode
{
		char id[100];
		struct IDNode* next;
};

struct LiteralNode
{
		char literal[MAX_LITERAL_LENGTH];
		struct LiteralNode* next;
};

typedef struct Credential
{
		PublicKey_t 			public_key;
		CertificateNode_t		cert_list;
} Credential_t;

Credential_t* parse_credential(const char* cred_file_name);

#endif /* __ACS_H__ */
