/* g++ -ggdb -o acceptor -L/usr/lib64/ -lACE ace_acceptor.cpp */
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <streambuf>

#include "ace/Auto_Ptr.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/Time_Value.h"
#include "ace/Codecs.h"
#include "ace/Date_Time.h"

#include "acs_server.h"
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/md5.h>
#include <openssl/evp.h>

namespace {

	unsigned DEFAULT_PORT = 5420;
	const unsigned MAX_RECV_BUFSIZE = (1024 * 16);
	const unsigned MAX_CRED_SIZE = (50 * 1024);
	const unsigned DEFAULT_RECV_TIMEOUT = 600;
	const unsigned DEFAULT_SEND_TIMEOUT = 200;
	const unsigned MAX_CHALLENGE_LENGTH = 32;

	const char CRED_END_MARKER[] = "-----END MPKI CREDENTIAL-----";
	const char RESPONSE_START_MARKER[] = "-----BEGIN MPKI CHALLENGE RESPONSE-----";
	const char RESPONSE_END_MARKER[] = "-----END MPKI CHALLENGE RESPONSE-----";
	const char REQUEST_START_MARKER[] = "-----BEGIN REPOSITORY CLIENT REQUEST-----";
	const char REQUEST_END_MARKER[] = "-----END REPOSITORY CLIENT REQUEST-----";
	const char CHALLENGE_BLK_START[] = "-----BEGIN MPKI CHALLENGE-----";
	const char CHALLENGE_BLK_END[] = "\n-----END MPKI CHALLENGE-----";
	const char REPLY_BLK_START[] = "-----BEGIN REPOSITORY SERVER REPLY-----";
	const char REPLY_BLK_END[] = "-----END REPOSITORY SERVER REPLY-----";
	const char PUBLICKEY_START_MARKER[] = "-----BEGIN PUBLIC KEY-----\n";
	const char PUBLICKEY_END_MARKER[] = "-----END PUBLIC KEY-----";
	const char ACCESS_GRANTED_STR[] = "granted";
	const char ACCESS_DENIED_STR[] = "denied";

	std::string JAVA_EXECUTABLE_PATH = "/usr/bin/java";
	std::string XSB_EXECUTABLE_PATH = "/usr/bin/xsb";
	std::string SERVER_POLICY_FILE;

	typedef enum {  Done = 0, 
					RecvTimedOut = 1, 
					RecvError = 2, 
					SendTimedOut = 3, 
					SendError = 4, 
					RandomChallengeError = 5, 
					BadCredential = 6,
					PubKeyDecodeFailed = 7,
					BadChallengeResponse = 8,
					TooLargeCredential = 9,
					ClientAuthFailed = 10,
					ClientCredTampered = 11,
					ClientCertExpired = 12,
					CertImportFailed = 13,
					TooLargeBuf = 14,
					IllegalProtocolState 
			 	 } ServiceStatus; 
	
	const char* errorStrings[] = {
		"Success",
		"Receive Timed Out",
		"Error Receiving Data",
		"Send Timed Out",
		"Error Sending Data",
		"Error in generating random challenge block",
		"Bad Credential",
		"Base64 decoding of public key failed",
		"Bad Response to challenge",
		"Too large credential",
		"Client Authentication Failed",
		"Client Credential Tampered",
		"Certificate Expired",
		"Certificate Import Failed",
		"Too Large Data Received",
		"Illegal Protocol State"
	};

	typedef enum { DEBUG = 0, INFO = 1, WARNING = 2, ERROR = 3, FATAL = 4 } LogLevel;

	typedef enum { 	CredExchange = 0, 
					Challenge = 1, 
					Response = 2, 
					Request = 3, 
					Reply =  4, 
					CloseConn = 5,
				    ProtocolDone = 6
				 } ProtocolState;

	const char* logLiterals[] = { "<DEBUG>", "<INFO>", "<WARNING>", "<ERROR>", "<FATAL>" };

	void printLogMsg(const std::ostringstream& logMsg, const LogLevel ll = DEBUG)
	{
		if( ll == DEBUG || ll == INFO ) {
			std::cout << logLiterals[ll] << logMsg.str() << std::endl;
		}
		else {
			std::cerr << logLiterals[ll] << logMsg.str() << std::endl;
		}
	}

	typedef std::vector<std::string> CmdLineArgs;
	typedef std::vector<std::string> LiteralList;
	typedef std::vector<std::string> ClauseList;

	ServiceStatus serveClient(ACE_SOCK_Stream& clientSock);
	int generateRandomBlock(char* blk, const unsigned blk_len);
	std::string parseResponse(const std::string& responseBlk);
	std::string parseRequest(const std::string& reqBlk);
	RSA* getRSAKey(const char* key);
	std::string getCommandOutPut(const std::string& cmd, CmdLineArgs& cmdArgs, 
								unsigned char* stdinput, const unsigned stdinputlength);

	bool javaSignatureVerify(const std::string& pubkey, 
					const std::string& msg, const std::string& signature);

	bool verifySignature(RSA* pubKey, const std::string& msg, const std::string& signature, 
							bool base64decodeSignature = true);

	void parseLiterals(const char* clause, LiteralList& llist);
	void transformLiteral(const std::string& pk, const LiteralList& llist, LiteralList& transformedList);
	std::string constructClause(const LiteralList& transformedList);
	bool isCredentialExpired(const Credential_t* pCred);
	const unsigned char* GetMD5Hash(const unsigned char* msg, const unsigned msgLength);
	const unsigned char* GetMD5Hash(const std::string& msg);
	std::string getDATimeClause(const TimeTuple_t& tm);
	const char* GetMD5Sum(const unsigned char* md5digest);

	ServiceStatus receiveBuffer(ACE_SOCK_Stream& clientSock, const std::string& endMarker,
						std::ostringstream& recvBuf, const unsigned max_recv_size = MAX_RECV_BUFSIZE);
	ServiceStatus sendBuffer(ACE_SOCK_Stream& clientSock, const std::string& buf);

	void checkExecutables();
	bool GetRequestParamaeters(const std::string& req, std::string& operation, std::string& filename);
};

#define LOG(msg, level)												\
do {																\
		std::ostringstream str;										\
		str << __FILE__ << "(" << __LINE__ << "): " << msg;			\
		printLogMsg(str, level);									\
} while(0)
	
//#define USE_JAVA_SIGNATURE_VERIFICATION 

#ifdef USE_JAVA_SIGNATURE_VERIFICATION
#define VERIFY_SIGNATURE(PK, MSG, SIGN) 										\
do {																			\
	if( false == javaSignatureVerify(PK, MSG, SIGN) ) {							\
		LOG("Client Authentication Failed.", DEBUG); 							\
		return ClientAuthFailed;												\
	}																			\
}while(0)
#endif

#ifndef USE_JAVA_SIGNATURE_VERIFICATION
#define __VERIFY_SIGNATURE(PK, MSG, SIGN, DECODE_SIGN)							\
do {																			\
	std::string pk = std::string(PUBLICKEY_START_MARKER) + PK 					\
					 + std::string(PUBLICKEY_END_MARKER);						\
																				\
	RSA* rsaKey = getRSAKey(pk.c_str());										\
	if( NULL == rsaKey ) {														\
		LOG("Failed to get RSA key", ERROR);									\
		return ClientAuthFailed;												\
	}																			\
	if( false == verifySignature(rsaKey, MSG, SIGN, DECODE_SIGN) ) {			\
		LOG("Client Authentication Failed.", DEBUG); 							\
		RSA_free(rsaKey);														\
		return ClientAuthFailed;												\
	}																			\
	RSA_free(rsaKey);															\
}while(0)

#define VERIFY_SIGNATURE(PK, MSG, SIGN)	__VERIFY_SIGNATURE(PK, MSG, SIGN, true)
#endif

int main(int argc, char* argv[])
{
	std::ostringstream serverCmdLine;
   	serverCmdLine << argv[0];
	if( argc < 2 ) {
	  	const std::string usage = std::string("Usage: ./acs <policy file name> [<listen port>]"
						 	      " [<xsb executable path>] [<java executable path>] " 
							      "\n\nDefault listen port is 5420. \nDefault XSB executable path ") + 
								  XSB_EXECUTABLE_PATH + std::string("\nDefault JAVA executable path ") + 
								  JAVA_EXECUTABLE_PATH;

		std::cerr << usage << std::endl;
		exit(-1);
	}

	if( -1 == access(argv[1], F_OK) ) {
		LOG("Server policy file " << argv[1] << " doesn't exist", ERROR);
		exit(-1);
	}
	serverCmdLine << " " << argv[1] ;
	

	if( 3 == argc ) {
		int port_arg = atoi(argv[2]);
		if( port_arg < 0 || port_arg > 65535) {
			LOG("Incorrect port " << port_arg << " passed in as cmd argument.", ERROR);
			exit(-1);
		}
		DEFAULT_PORT = port_arg;
	}
	serverCmdLine << " " << argv[2];

	if( 4 == argc ) {
		if( -1 == access(argv[3], X_OK) ) {
			LOG("XSB exe " << argv[3] << " either doesn't exist or it is not executable", ERROR);
			exit(-1);
		}
		XSB_EXECUTABLE_PATH = std::string(argv[3]);
	}

	if( argc < 4 )
		checkExecutables();

	serverCmdLine << " " << argv[3];

	if( 5 == argc ) {
		if( -1 == access(argv[4], X_OK) ) {
			LOG("JAVA exe " << argv[4] << " either doesn't exist or it is not executable", ERROR);
			exit(-1);
		}
		JAVA_EXECUTABLE_PATH = std::string(argv[4]);
	}
	serverCmdLine << " " << argv[4];

	SERVER_POLICY_FILE = std::string(argv[1]);
	LOG("Server Cmd Line: " << serverCmdLine.str(), INFO); 
	LOG("Using Server Policy File: " << SERVER_POLICY_FILE, INFO); 

	ACE_INET_Addr serverAddr;
	ACE_SOCK_Acceptor acceptor;

	if(serverAddr.set(DEFAULT_PORT) == -1) {
		LOG("Failed bind() on port "<< DEFAULT_PORT, FATAL );
		return -1;
	}

	if(acceptor.open(serverAddr) == -1) {
		LOG("Failed listen() on port "<< DEFAULT_PORT, FATAL );
		return -1;
	}
	LOG("Server started listening on port " << DEFAULT_PORT, INFO);

	ACE_SOCK_Stream peer;
	ACE_INET_Addr peerAddr;
	while(1) {
		if(acceptor.accept(peer, &peerAddr) == -1) {
			LOG("Accept failed", WARNING);
			continue;
		}

		char addr[64] = { 0 };
		if( AF_INET == peerAddr.get_type() ) {
				peerAddr.addr_to_string(addr, sizeof(addr));
				LOG("Client: " << addr << " connected.", INFO );
		}

		const ServiceStatus status = serveClient(peer);
		LOG( "Client: " << addr << " Status: " << status << " - " << errorStrings[status], INFO );
	}

	return acceptor.close () == -1 ? 1 : 0;
}

namespace {

/** 
* @brief 
The function checks if the xsb engine exists in the path mentioned while invoking the server
If it doesn't exist in the path mentioned , this function points out which xsb is used and prints the executable path
*/
void checkExecutables()
{
	if( -1 == access(XSB_EXECUTABLE_PATH.c_str(), X_OK) ) {
		LOG("Default XSB Executable " << XSB_EXECUTABLE_PATH << "either doesn't exist or user doesn't have execute permission for it.", ERROR);
	}
	else {
		return;
	}

	CmdLineArgs args;
	args.push_back("xsb");
	std::string xsbpath = getCommandOutPut("/usr/bin/which", args, NULL, 0);
	while( std::string::npos != xsbpath.find("\n") ) {
		xsbpath.erase(xsbpath.find("\n"));
	}

	if( (std::string::npos == xsbpath.find("no xsb")) ||  
		(std::string::npos == xsbpath.find("")) ) {

		LOG("Using XSB Executable " << xsbpath, INFO);
		XSB_EXECUTABLE_PATH = xsbpath;
	}
	else {
		exit(-1);
	}
}

/** 
* @brief 
* 
* @param clientSock is the socket in which the client sends the buffer
* @param endMarker is the marker which tells when to stop receiving the buffer
* @param recvBuf is the buffer received from teh client
* @param max_recv_size is the max size of the buffer which serevr can accept from client 
* 
* @return 
*/
ServiceStatus receiveBuffer(ACE_SOCK_Stream& clientSock, const std::string& endMarker,
						std::ostringstream& recvBuf, const unsigned max_recv_size /*= MAX_RECV_BUFSIZE*/)
{
	const unsigned MAX_BUF_SIZE = 4096;
	const ACE_Time_Value recvTimeOut(DEFAULT_RECV_TIMEOUT, 0);
	int bytesRecvd = 0;
	char buffer[MAX_BUF_SIZE] = { 0 };

	memset(buffer, 0, sizeof(buffer));

	while(0 < (bytesRecvd = clientSock.recv(buffer, sizeof(buffer), 0, &recvTimeOut))) {
		recvBuf << buffer;
		if( std::string::npos != recvBuf.str().find(endMarker) ) {
			break;
		}
		if( recvBuf.str().length() > max_recv_size ) {
			return TooLargeBuf;
		}
		memset(buffer, 0, sizeof(buffer));
	}

	if( -1 == bytesRecvd ) {
		if( ETIME == errno ) {
			return RecvTimedOut;
		}

		LOG("recv() returned -1 with errno: " << errno, ERROR);
		return RecvError;
	}

	return Done;
}

/** 
* @brief 
* 
* @param clientSock is the socket connection with the client
* @param buf  is the buffer that is sent to the client
* 
* @return 
*/
ServiceStatus sendBuffer(ACE_SOCK_Stream& clientSock, const std::string& buf)
{
	const ACE_Time_Value sendTimeOut(DEFAULT_SEND_TIMEOUT, 0);
	int bytesSent = 0;
	const unsigned bufLength = buf.length();
	const char* bufPtr = buf.c_str();
	unsigned totalBytesSent = 0;

	LOG(buf, INFO);

	while( 0 < (bytesSent = clientSock.send(bufPtr + totalBytesSent,
											 (bufLength - totalBytesSent),
											 &sendTimeOut)) ) {
		totalBytesSent += bytesSent;
		if( totalBytesSent >= bufLength ) {
			break;
		}
	}

	if( -1 == bytesSent ) {
		if( ETIME == errno ) {
			return SendTimedOut;
		}

		LOG("send() returned -1 with errno: " << errno, ERROR);
		return SendError;
	}

	return Done; 
}



/** 
* @brief This function handles does handshake with the client, all the negotiations with the clients 
* 
* @param clientSock is the socket connection with the client
* 
* @return the protocol status of the request  
*/
ServiceStatus serveClient(ACE_SOCK_Stream& clientSock)
{
	ProtocolState state = CredExchange;
	std::ostringstream clientRequestBuf;
	Credential_t* pCred = NULL;
	std::string challengeBlk;
	std::string consolidatedPolicyFile;
	std::string xsbQueryResult;
	char random_challege[MAX_CHALLENGE_LENGTH + 1] = { 0 };
	ServiceStatus stat;
	clientSock.disable(ACE_NONBLOCK); // Ensure blocking <recv>.
	std::string pubk_Hash;

	while( state != ProtocolDone ) {
		clientRequestBuf.str("");
		switch( state ) 
		{
		case CredExchange:
			clientRequestBuf.str("");
			if( Done != (stat = receiveBuffer(clientSock, CRED_END_MARKER, 
												clientRequestBuf, MAX_CRED_SIZE)) ) {
				if( TooLargeBuf == stat ) {
					LOG("Too Large Credential", ERROR);
					stat = TooLargeCredential;
				}
				else{
					LOG("receiveBuffer failed with: " << stat, ERROR);
				}
				return stat;
			}

			{
				LOG("\nRecvd Client Credential: \n" << clientRequestBuf.str(), INFO);

				const char* cred_file = "credential.txt";
				std::ofstream credFile(cred_file);
				credFile << clientRequestBuf.str();
				credFile.close();

				if( pCred = parse_credential(cred_file) ) {
					LOG("\n Base64 Encoded Client PK: \n" << pCred->public_key.base64_pk, INFO);
				}
				else {
					LOG("\n FAILED TO PARSE CREDENTIAL. SEEMS IT IS BAD!!!\n", DEBUG);
					return BadCredential;
				}

				if( false == isCredentialExpired(pCred) ) {
					LOG("\n CERTIFICATE EXPIRED \n", DEBUG);
					return ClientCertExpired;
				}

				state = Challenge;
			}
			break;

		case Challenge:
			{
				if( 0 != generateRandomBlock(random_challege, MAX_CHALLENGE_LENGTH) ) {
					LOG( "generateRandomBlock() failed", ERROR );
					return RandomChallengeError;
				}
				
				challengeBlk = std::string(CHALLENGE_BLK_START) + std::string("\n") + 
						std::string(random_challege) + std::string(CHALLENGE_BLK_END);

				if( Done != (stat = sendBuffer(clientSock,challengeBlk)) ) {
					LOG("sendBuffer failed with: " << stat, ERROR);
					return stat;
				}
			}
			state = Response;
			break;

		case Response:

			clientRequestBuf.str("");
			if(Done != (stat = receiveBuffer(clientSock, RESPONSE_END_MARKER, clientRequestBuf))) {
				LOG("receiveBuffer failed with: " << stat, ERROR);
				return stat;
			}

			{
				LOG( "\nReceived Challenge Reponse from Client \n" << clientRequestBuf.str(), INFO );
				std::string signedChallenge = parseResponse(clientRequestBuf.str());
				if( std::string("") == signedChallenge ) {
					return BadChallengeResponse;
				}
			
				VERIFY_SIGNATURE( std::string(pCred->public_key.base64_pk),
									std::string(random_challege), signedChallenge );
				LOG("Client Authenticated" , INFO);

				size_t pubk_length = 0;
				unsigned char* pubk_base64 = 
						ACE_Base64::decode((const ACE_Byte*)pCred->public_key.base64_pk, &pubk_length);
				if( NULL == pubk_base64 ) {
					LOG( "Base64 Decoding of " << pCred->public_key.base64_pk << " Failed", ERROR);
					return PubKeyDecodeFailed;
				}

				const char* pubk_base64MD5Sum = GetMD5Sum(GetMD5Hash(pubk_base64, pubk_length));
				if( NULL == pubk_base64MD5Sum ) {
						return CertImportFailed;
				}
				pubk_Hash = pubk_base64MD5Sum;
				pubk_Hash = std::string("rsa_") + pubk_Hash;
				LOG("Client Public Key Hash " << pubk_Hash, INFO);

				CertificateNode_t certptr = pCred->cert_list; 
				if( NULL == certptr ) {
					LOG( "No certificates in the credential" , FATAL );
					return BadCredential;
				}

				ClauseList clList;
				while( certptr ) {
					size_t pklength = 0;
					unsigned char* pkbase64 = 
						ACE_Base64::decode((const ACE_Byte*)certptr->cert.public_key.base64_pk, &pklength);
					if( NULL == pkbase64 ) {
						LOG( "Base64 Decoding of " << certptr->cert.public_key.base64_pk << " Failed", ERROR);
						return PubKeyDecodeFailed;
					}

					const char* pkbase64MD5Sum = GetMD5Sum(GetMD5Hash(pkbase64, pklength));
					if( NULL == pkbase64MD5Sum ) {
						return CertImportFailed;
					}
					std::string pkHash(pkbase64MD5Sum);
					pkHash = std::string("rsa_") + pkHash;
					LOG("PkHash " << pkHash , INFO);
					
					LiteralList llist, transformedList;
					std::ostringstream certClauses;
					ClauseNode_t clauseptr = certptr->cert.clause_list;
					while( clauseptr ) {
						llist.clear(); transformedList.clear(); 
						LOG("Clause: " << clauseptr->cl.clause, INFO);

						certClauses << clauseptr->cl.clause;
						parseLiterals(clauseptr->cl.clause, llist);
						transformLiteral(pkHash, llist, transformedList);

						std::string trClause = constructClause(transformedList);
						clList.push_back(trClause);
						clauseptr = clauseptr->next;
					}

					if( certptr->cert.validity_list ) {
						certClauses << getDATimeClause(certptr->cert.validity_list->not_before);
						certClauses << getDATimeClause(certptr->cert.validity_list->not_after); 
					}

					VERIFY_SIGNATURE( std::string(certptr->cert.public_key.base64_pk),
									  certClauses.str(), std::string(certptr->cert.signature.sign) );

					certptr = certptr->next;
				}

				std::ifstream ifpolicy(SERVER_POLICY_FILE.c_str());
				consolidatedPolicyFile = std::string("consol_") + SERVER_POLICY_FILE;
				std::ofstream ofpolicy( consolidatedPolicyFile.c_str() );
				std::ostringstream policystream;

				std::copy( std::istreambuf_iterator<char>(ifpolicy), std::istreambuf_iterator<char>(), 
							std::ostreambuf_iterator<char>(policystream) );

				ifpolicy.close();
				ofpolicy << policystream.str();
				ofpolicy << "\n\n";

				LOG("\nTransformed Clauses :- \n###################################\n", INFO);
				for(ClauseList::iterator itr = clList.begin(); itr != clList.end(); ++itr) {
						LOG("Clause: " << *itr, INFO);
						ofpolicy << *itr << "\n\n";
				}
				LOG("\n###################################\n", INFO);
				ofpolicy.close();
			}
			state = Request;
			break;

		case Request:
		
			clientRequestBuf.str("");
			if( Done != (stat = receiveBuffer(clientSock, REQUEST_END_MARKER, 
												clientRequestBuf)) ) {
				LOG("receiveBuffer failed with: " << stat, ERROR);
				return stat;
		    }

		    {
				LOG( "\nReceived Request: " << clientRequestBuf.str(), INFO );
				const std::string reqBlk = clientRequestBuf.str();
				std::string req = parseRequest(reqBlk);

				if( req == std::string("Exit\n") || req == std::string("exit\n") ) {
					state = CloseConn;	
					continue;
				}
					 
				while( std::string::npos != req.find("\n") ) {
					req.erase(req.find("\n"));
				}
				while( std::string::npos != req.find(".") ) {
					req.erase(req.find("."));
				}

				std::string xsbQuery("allow(");
				xsbQueryResult = "";
				if( std::string("") == req ) {
					xsbQueryResult = "IncorrectRequest";
				}
				else {
					std::string operation, fileName;
					if( false == GetRequestParamaeters(req, operation, fileName) ) {
						LOG( "\nMalformed Request: " << req, ERROR);
						xsbQueryResult = "IncorrectRequest";
					}
					else {
						xsbQuery += pubk_Hash;
						xsbQuery += ",";
						xsbQuery += operation;
						xsbQuery += ",";
						xsbQuery += fileName;
						xsbQuery += ").";
						LOG("\nTransformed XSB Query: " << xsbQuery, INFO);
					}
				}

				LOG( "\nRequest: " << req, INFO );

				///usr/local/bin/xsb  --nobanner --quietload --noprompt -e "[consol_server_policy2],allow(jeremiah, read, file3)." -e "halt."
				if( xsbQueryResult == std::string("") ) {

					CmdLineArgs args;
					args.push_back(std::string("--nobanner")); 
					args.push_back(std::string("--quietload"));
					args.push_back(std::string("--noprompt")); 
					args.push_back(std::string("-e"));

					std::string xsbInputFile = consolidatedPolicyFile;
					if( std::string::npos != xsbInputFile.find(".P") ) {
						xsbInputFile.erase(xsbInputFile.find(".P"));
					}

					std::string goal = std::string("[") + xsbInputFile + 
									std::string("],")  + xsbQuery;

					args.push_back(goal);
					args.push_back(std::string("-e"));
					args.push_back(std::string("halt."));

					xsbQueryResult = getCommandOutPut(XSB_EXECUTABLE_PATH, args, NULL, 0);
					LOG( "Got Result: " << xsbQueryResult, INFO );
				}
			}	

			state = Reply;
			break;

		case Reply:
		{
			std::string accessResult( (std::string::npos != xsbQueryResult.find("yes")) ? 
								ACCESS_GRANTED_STR : ACCESS_DENIED_STR );

			const std::string replyBlk = std::string(REPLY_BLK_START) + std::string("\n") + 
						accessResult + std::string("\n") + std::string(REPLY_BLK_END);

			if( Done != (stat = sendBuffer(clientSock, replyBlk)) ) {
				LOG("sendBuffer failed with: " << stat, ERROR);
				return stat;
			}
		}
		state = Request;
		break;

		case CloseConn:
			clientSock.close();
			state = ProtocolDone;
			break;

		default:
			LOG("Invalid Protocol State", ERROR);
			clientSock.close();
			return IllegalProtocolState;
		}
	}

	clientSock.close();
	return Done;
}

/** 
* @brief Generates random block of given length having only a-zA-Z chars.
* 
* @param blk - [out] upon successful return this block contains random data.
* @param blk_len - [in] lenght of the block.
* 
* @return 0 upon success, -1 on failure.
*/
int generateRandomBlock(char* blk, const unsigned blk_len)
{
	//@todo - use ACE_Auto_Ptr instead for buf.
	unsigned char* buf = new unsigned char[blk_len];

	const int status = RAND_bytes(buf, blk_len);
	if( 1 != status ) {
		LOG( "Rand Bytes failed with: " << status, ERROR );
		delete[] buf;
		return -1;
	}

	unsigned char strt = 0;
	for(unsigned i = 0; i < blk_len; ++i) {
		strt = (buf[i] % 25);
		if( (buf[i] % 2) ) {
			buf[i] = strt + 65;
		}
		else {
			buf[i] = strt + 97;
		}
	}
	memcpy(blk, buf, blk_len);
	delete[] buf;
	return 0;
}

/** 
* @brief 
* 
* @param reqBlk - [in] Contains the request messages with the start and end markers
* 
* @return the request block without the start and the end markers
*/
std::string parseRequest(const std::string& reqBlk) 
{
	size_t startMarkerPos = reqBlk.find(REQUEST_START_MARKER);
	const size_t endMarkerPos =  reqBlk.find(REQUEST_END_MARKER);
	if( std::string::npos == startMarkerPos ||
		std::string::npos == endMarkerPos ) {
		LOG( "Bad Request Block. Either block start or block end marker is missing", ERROR );
		return "";
	}

	startMarkerPos += strlen(REQUEST_START_MARKER);
	while( '\n' == reqBlk.at(startMarkerPos) ) {
			startMarkerPos++;
	}

	const std::string req = reqBlk.substr(startMarkerPos, endMarkerPos - startMarkerPos);
	return req;
}


/** 
* @brief 
* 
* @param responseBlk contains the reponse block with the start and end markers
* 
* @return the response block without the start and end markers
*/
std::string parseResponse(const std::string& responseBlk)
{
	size_t startMarkerPos = responseBlk.find(RESPONSE_START_MARKER);
	const size_t endMarkerPos =  responseBlk.find(RESPONSE_END_MARKER);     
	if( std::string::npos == startMarkerPos ||
		std::string::npos == endMarkerPos ) {
		LOG( "Bad Response Block. Either block start or block end marker is missing", ERROR );
		return NULL;
	}

	startMarkerPos += strlen(RESPONSE_START_MARKER);
	while( '\n' == responseBlk.at(startMarkerPos) ) {
			startMarkerPos++;
	}

	const unsigned resBlkLength = endMarkerPos - startMarkerPos;
	std::string base64Response = responseBlk.substr(startMarkerPos, resBlkLength);
	return base64Response;
}

RSA* getRSAKey(const char* key)
{
		char* k = const_cast<char*>(key);
		BIO *keybio = BIO_new_mem_buf(k, -1);
		if(keybio == NULL) {
			return NULL;
		}

		RSA* rsakey = PEM_read_bio_RSA_PUBKEY(keybio, NULL, NULL, NULL);
		BIO_free(keybio);
		return rsakey;
}


/** 
* @brief This function executes the given command,reads and returns the output to the caller 
* 
* @param cmd - [in] The command to be executed
* @param cmdArgs - [in] The arguments given to the command for execution
* @param stdinput - [in] The input that the standard input expects while executing the command 
* @param stdinputlength - [in] The length of the string stdinput
* 
* @return the output of the executed command is returned back
*/
std::string getCommandOutPut(const std::string& cmd, CmdLineArgs& cmdArgs, 
								unsigned char* stdinput, const unsigned stdinputlength)
{
	int outfd[2];
	int infd[2];
	int oldstdin, oldstdout;

	pipe(outfd); // Where the parent is going to write to
	pipe(infd); // From where parent is going to read

	oldstdin = dup(0); // Save current stdin
	oldstdout = dup(1); // Save stdout

	close(0);
	close(1);

	dup2(outfd[0], 0); // Make the read end of outfd pipe as stdin
	dup2(infd[1], 1); // Make the write end of infd as stdout

	if(!fork()) {

		char **argv = new char*[2 + cmdArgs.size()];
		argv[0] =  (char*) cmd.c_str();
		int i = 1;
		for(CmdLineArgs::iterator itr = cmdArgs.begin(); itr != cmdArgs.end(); ++itr) { 
			argv[i++] = (char*) itr->c_str();
			//std::cerr << "Arg : \n"  << itr->c_str() << "\n";
		}
		argv[i] = 0;

		close(outfd[0]); // Not required for the child
		close(outfd[1]);
		close(infd[0]);
		close(infd[1]);

		execv(argv[0],argv);
	}
	else {
		close(0); close(1);
		dup2(oldstdin, 0);
		dup2(oldstdout, 1);

		close(outfd[0]); // These are being used by the child
		close(infd[1]);

		if( stdinput && stdinputlength ) {
			write(outfd[1],stdinput, stdinputlength ); // Write to child stdin
			close(outfd[1]);
		}
		
		char input[1024] = { 0 };
		int bytesRead = 0;
		std::string retVal;
		while( bytesRead = read(infd[0], input, 1023) ) {
			retVal += std::string(input);
			memset(input, 0, sizeof(input));
		}
		return retVal;
	}
}

void parseLiterals(const char* clause, LiteralList& llist)
{
	if( NULL == clause ) 
		return;

	std::string cls(clause);
	size_t marker;

	size_t pos = cls.find(":-");
	if( std::string::npos != pos ) {
		std::string literal = cls.substr(0, pos);
		llist.push_back(literal);
		pos += strlen(":-");
	}
	else {
			pos = 0;
	}

	while(1) {
		marker = cls.find("),", pos);
		if( marker != std::string::npos ) {
			marker += 1;
			std::string literal = cls.substr(pos, marker-pos);
			llist.push_back(literal);
			pos = marker + 1;
			continue;
		}

		marker = cls.find(").", pos);
		if( marker != std::string::npos ) {
			marker += 1;
			std::string literal = cls.substr(pos, marker-pos);
			llist.push_back(literal);
			break;
		}
	}
}

void transformLiteral(const std::string& pk, const LiteralList& llist, LiteralList& transformedList)
{
		for(LiteralList::const_iterator itr = llist.begin(); itr != llist.end(); ++itr) {
			std::string transformedLiteral("says(");

			size_t pos = itr->find("says(");
			if( std::string::npos == pos ) {
				transformedLiteral += pk;
				transformedLiteral += ",";
				transformedLiteral += *itr;
				transformedLiteral += ")";
				transformedList.push_back(transformedLiteral);
			}
			else {
				transformedList.push_back(*itr);
			}
		}
}

std::string constructClause(const LiteralList& transformedList) 
{
	size_t listSize = transformedList.size();

	if( 0 == listSize ) {
			return std::string("");
	}

	if( 1 == listSize ) {
		return transformedList[0] + ".";
	}

	std::string clause = transformedList[0] + ":-";

	for(int i = 1; i < listSize - 1; ++i) {
			clause += transformedList[i];
			clause += std::string(",");
	}

	clause += transformedList[listSize - 1];
	clause += std::string(".");
	return clause;
}

bool javaSignatureVerify(const std::string& pubkey, 
				const std::string& msg, const std::string& signature) 
{
	CmdLineArgs javaArgs;
	javaArgs.push_back("checkSignature");
	javaArgs.push_back(pubkey);
	javaArgs.push_back(msg);
	javaArgs.push_back(signature);

	std::string signVerificationMsg = 
			getCommandOutPut(JAVA_EXECUTABLE_PATH, javaArgs, NULL, 0);

	if( std::string::npos == signVerificationMsg.find("verified") ) {
		return false;
	}
	return true;
}

/** 
* @brief 
* 
* @param pCred The credential for which the function checks if the crenetial is expired
* 
* @return  boolean if the credential is expired or not
*/
bool isCredentialExpired(const Credential_t* pCred)
{
	if( !pCred || !pCred->cert_list ) {
		return false;
	}

	CertificateNode_t certptr = pCred->cert_list;
	while( certptr ) {

		if( certptr->cert.validity_list ) {

			TimeTuple_t notbefore = certptr->cert.validity_list->not_before;
			TimeTuple_t notafter = certptr->cert.validity_list->not_after;
			ACE_Date_Time curTime;

			struct tm curtime, after, before;
			curtime.tm_sec = curTime.second();
			curtime.tm_min = curTime.minute();
			curtime.tm_hour = curTime.hour();
			curtime.tm_mday = curTime.day();
			curtime.tm_mon  = curTime.month();
			curtime.tm_year = curTime.year();

			after.tm_sec = notafter.sec;
			after.tm_min = notafter.min;
			after.tm_hour = notafter.hour;
			after.tm_mday =  notafter.day;
			after.tm_mon  =	notafter.month;
			after.tm_year = notafter.year;

			before.tm_sec = notbefore.sec;
			before.tm_min = notbefore.min;
			before.tm_hour = notbefore.hour;
			before.tm_mday = notbefore.day;
			before.tm_mon  = notbefore.month;
			before.tm_year = notbefore.year;

			time_t epoch_cur = mktime(&curtime);
			time_t epoch_after = mktime(&after);
			time_t epoch_before  = mktime(&before);

			if( epoch_cur <= epoch_after && epoch_cur >= epoch_before ) {
				return true;
			}
			else {
				LOG("Invalid Certificate", DEBUG);
				return false;
			}

			certptr = certptr->next;
		}
	}

	return true;
}

const char* GetMD5Sum(const unsigned char* md5digest) 
{
	if( NULL == md5digest ) {
		return NULL;
	}

	static char md5sum[2*MD5_DIGEST_LENGTH + 1];
	memset(md5sum, 0, sizeof(md5sum));

	for(int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		sprintf(&md5sum[2*i], "%02x", md5digest[i]);	
	}

	return md5sum;
}

std::string getDATimeClause(const TimeTuple_t& tm) 
{
	std::ostringstream clause;
	clause << "datime(";
	clause << tm.year << "," << tm.month << "," << tm.day << ","
			<< tm.hour << "," << tm.min << "," << tm.sec << ").";
	return clause.str();
}

const unsigned char* GetMD5Hash(const unsigned char* msg, const unsigned msgLength)
{
	static unsigned char digest[MD5_DIGEST_LENGTH] = { 0 }; 
	MD5_CTX md5_ctx = { 0 };
	memset(digest, 0, sizeof(digest));

	if( !msg || !msgLength ) {
		return NULL;
	}

	int rc = MD5_Init(&md5_ctx);
	if(1 != rc) { 
		LOG("MD5_Init failed", ERROR);
		return NULL;
	}

	rc = MD5_Update(&md5_ctx, msg, msgLength);
	if(1 != rc) { 
		LOG("MD5_Update failed", ERROR);
		return NULL;
	}

	rc = MD5_Final(digest, &md5_ctx);
	if(1 != rc) { 
		LOG("MD5_Final failed", ERROR);
		return NULL;
	}
	
   	return digest;
}

const unsigned char* GetMD5Hash(const std::string& msg)
{
	return  GetMD5Hash((const unsigned char*) msg.c_str(), msg.length());
}

bool verifySignature(RSA* pubKey, const std::string& msg, const std::string& signature,
						bool base64decodeSignature /*= true*/)
{
	if( NULL == pubKey ) {
		LOG("Bad Arguments", ERROR);
		return false;
	}

	ACE_Auto_Array_Ptr<unsigned char> sign(NULL);
	size_t signLength = signature.length();

	if( base64decodeSignature ) {
		sign.reset( ACE_Base64::decode( (const ACE_Byte*)signature.c_str(), &signLength ) ); 
		if( NULL == sign.get() ) {
			LOG( "Base 64 Decode of Signed Challenge Failed", ERROR );
			return false;
		}
	}
	else {
		sign.reset(new unsigned char[signature.length() + 1]);
		strcpy((char*)sign.get(), signature.c_str());
	}

	MD5_CTX md5_ctx = { 0 };
	unsigned char digest[MD5_DIGEST_LENGTH]; 
	const unsigned char* dgst = GetMD5Hash(msg);
	if( NULL == dgst ) {
		return false;
	}
	memcpy(digest, dgst, sizeof(digest));

	return (1 == RSA_verify(NID_md5, digest, sizeof(digest), sign.get(), signLength, pubKey)); 
}


void stripWhiteSpaces( std::string& stringIn )
{
       std::string::size_type pos = 0;
       bool spacesLeft = true;
       while( spacesLeft )
       {
       pos = stringIn.find(" ");
       if( pos != std::string::npos )
       stringIn.erase( pos, 1 );
       else
       spacesLeft = false;
      }
}

unsigned int numberOfOccurences(const std::string& str,std::string marker)
{
	unsigned int count =0;
	size_t pos = str.find(marker);
	while(std::string::npos != pos)
	{
		count++;
		pos = str.find(marker,pos+1);
	}
	return count;
}

bool GetRequestParamaeters(const std::string& req, std::string& operation, std::string& filename)
{
	if((numberOfOccurences(req,"(") > 1) || (numberOfOccurences(req,")") > 1) || (numberOfOccurences(req,",") > 1))
	{
		return false;
	}
	size_t pos = req.find("request");
	if( std::string::npos == pos ) {
		return false;
	}
	
	size_t obpos = req.find("(", pos);
	size_t cbpos = req.find(")", pos);
	if( std::string::npos == obpos ||
		std::string::npos == cbpos ) {
		return false;
	}

	const unsigned reqParamLength = cbpos - obpos - 1;
	std::string reqParams = req.substr(obpos+1, reqParamLength);

	size_t commapos = reqParams.find(",", pos);
	if( std::string::npos == commapos ) {
		return false;
	}

	operation = reqParams.substr(0,commapos);
	filename = reqParams.substr(commapos+1, reqParamLength - commapos);
	stripWhiteSpaces(operation);
	stripWhiteSpaces(filename);

	return true;
}




}; //unnamed namespace

