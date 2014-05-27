/** 
 * @file main.cpp
 * @brief A file that implements simple client server test cases using Active Object.
 * @author Manojkumar M Waghmare
 * @version 1.0
 * @date 2011-04-17
 */
#include "ace/OS.h"
#include "Peer.h"
#include "ObjProxy.h"
#include "MemFunctor.h"
#include "ace/Future.h"
#include "ace/Atomic_Op.h"
#include <iostream>
#include <sstream>

//Following are the macros/control patterns that make it easy to write test cases. 

#define ASSERT( cond )				\
do {						\
    if( !( cond ) ) {				\
	std::cerr << __FILE__ << ": " << __LINE__ << " Failed Assertion: " 	\
	<< #cond  << std::endl;							\
    }										\
} while(0)


#define THREAD_EXECUTE(threadproxy, ...) 	threadproxy( MemFunctor(__VA_ARGS__) )

#define THREAD_RET_EXECUTE(threadproxy, rettype, retvar, ...)							\
	ACE_Future<rettype> retvar;															\
	{																					\
		rettype __dummy__;																\
		retvar = threadproxy( RetMemFunctor(__dummy__, __VA_ARGS__) );					\
	}

#define LOG(msg) 													\
	do {															\
		std::ostringstream ostr;									\
		ostr << __FILE__ << "(" << __LINE__ << "): " << msg;		\
		logger( MemFunctor(&Logger::log, ostr.str()) );				\
	} while(0)																

class Logger
{
	public:
		int log(const std::string& logstr) {
			std::cerr << logstr << std::endl;
			return 0;
		}
};

int testServerRead();
int testServerWrite();
int testAlternateReadWrite();

const int INVALID_SOCKET = -1;
unsigned PORT_NUM = 12122;
ObjProxy<Logger> logger; //Create a logger which executes log method in different thread of execution.

void getLocalSockAddr(const int portNum, struct sockaddr_in& saddr) 
{
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(portNum);
}


int main(int , char *[])
{
    testServerRead();
    testServerWrite();
    testAlternateReadWrite();
    std::cerr << "All Tests Executed. Done!!!" << std::endl;
    logger.exit();
}


/** 
 * @brief Test server read operation.
 * 
 * @return 0 if success, negative values otherwise.
 */
int testServerRead()
{
    const char DATA[] = { "DEADBEEF" };
    struct sockaddr_in serv_addr, their_addr;

    LOG("#### testServerRead() ####");

    ObjProxy<Peer> client;  //Create a proxy for Peer which executes Peer methods in seperate
    //thread of execution

    getLocalSockAddr(PORT_NUM, serv_addr);

    //Create a socket
    const int sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if( sock == INVALID_SOCKET ) { 
	LOG("socket() failed with errno: " << errno);
	return -1;
    }

    //Configure and listen on server
    int  optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    socklen_t sockln = sizeof(serv_addr);

    if( -1 == ::bind(sock, (struct sockaddr *) &serv_addr, sockln) ) {
	LOG("bind() failed with errno: " << errno);
	return -1;
    }

    //Listen
    ::listen( sock, 5 );

    //Ask client to connect where server will be listening
    THREAD_EXECUTE(client, &Peer::connect, (sockaddr*)&serv_addr);

    //Accept Client's connect
    sockln = sizeof(their_addr);
    const int aFD = ::accept(sock, (struct sockaddr*)&their_addr, &sockln);
    ASSERT( aFD != INVALID_SOCKET );
    LOG("Accepted Request. FD: " << aFD);

    THREAD_RET_EXECUTE(client, int, bWritten, &Peer::write, DATA, sizeof(DATA));

    char Buf[24]= {0};
    read(aFD, Buf, sizeof(Buf));

    int bytesWritten;
    bWritten.get(bytesWritten);		//this call blocks until client thread finishes and returns
    ASSERT( bytesWritten == sizeof(DATA) );
    LOG("Client Write: " << Buf);
    ASSERT( 0 == strcmp(Buf, DATA) );
    LOG("Server Read: " << Buf);

    THREAD_EXECUTE(client, &Peer::close);
    client.exit();

    close(aFD);
    close(sock) ;
    return 0;
}

/** 
 * @brief Test server write operation works
 * 
 * @return 0 on success, negative values otherwise.
 */
int testServerWrite()
{
    const char DATA[] = { "DEADBEEF" };
    struct sockaddr_in serv_addr, their_addr;

    LOG("#### testServerWrite() ####");

    ObjProxy<Peer> client;  //Create a proxy for Peer which executes Peer methods in seperate
    //thread of execution

    getLocalSockAddr(PORT_NUM, serv_addr);

    //Create a socket
    const int sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if( sock == INVALID_SOCKET ) { 
	LOG("socket() failed with errno: " << errno);
	return -1;
    }

    //Configure and listen on server
    int  optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    socklen_t sockln = sizeof(serv_addr);

    if( -1 == ::bind(sock, (struct sockaddr *) &serv_addr, sockln) ) {
	LOG("bind() failed with errno: " << errno);
	return -1;
    }

    //Listen
    ::listen( sock, 5 );

    //Ask client to connect where server will be listening
    THREAD_EXECUTE(client, &Peer::connect, (sockaddr*)&serv_addr);

    //Accept Client's connect
    sockln = sizeof(their_addr);
    const int aFD = ::accept(sock, (struct sockaddr*)&their_addr, &sockln);
    ASSERT( aFD != INVALID_SOCKET );
    LOG("Accepted Request. FD: " << aFD);

    char ClientBuf[24]= {0};
    THREAD_RET_EXECUTE(client, int, bRead, &Peer::read, ClientBuf, sizeof(ClientBuf));

    const int bytesWritten = write(aFD, DATA, sizeof(DATA));
    ASSERT( sizeof(DATA) == bytesWritten );
    LOG("Server Write: " << DATA);

    int bytesRead;
    bRead.get(bytesRead); //block until client finishes.
    LOG("Client Read: " << ClientBuf);
    ASSERT( 0 == strcmp(ClientBuf, DATA) );

    THREAD_EXECUTE(client, &Peer::close);
    client.exit();

    close(aFD);
    close(sock) ;
    return 0;
}

/** 
 * @brief Test consecutive read and write operation.
 * 
 * @return 0 on success, negative values otherwise
 */
int testAlternateReadWrite()
{
    const char SERVER_DATA[] = { "IamServer" };
    const char CLIENT_DATA[] = { "IamClient" };
    char server_buf[30] = { 0 };
    char client_buf[30] = { 0 };
    struct sockaddr_in serv_addr, their_addr;

    LOG("#### testAlternateReadWrite() ####");

    ObjProxy<Peer> client;  //Create a proxy for Peer which executes Peer methods in seperate
    //thread of execution

    getLocalSockAddr(PORT_NUM, serv_addr);

    //Create a socket
    const int sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if( sock == INVALID_SOCKET ) { 
	LOG("socket() failed with errno: " << errno);
	return -1;
    }

    //Configure and listen on server
    int  optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    socklen_t sockln = sizeof(serv_addr);

    if( -1 == ::bind(sock, (struct sockaddr *) &serv_addr, sockln) ) {
	LOG("bind() failed with errno: " << errno);
    }

    //Listen
    ::listen( sock, 5 );

    //Ask client to connect where server will be listening
    THREAD_EXECUTE(client, &Peer::connect, (sockaddr*)&serv_addr);

    //Accept Client's connect
    sockln = sizeof(their_addr);
    const int aFD = ::accept(sock, (struct sockaddr*)&their_addr, &sockln);
    ASSERT( aFD != INVALID_SOCKET );
    LOG("Accepted Request. FD: " << aFD);

    int bytesWritten;
    //client writes & server reads
    THREAD_RET_EXECUTE(client, int, bWritten, &Peer::write, CLIENT_DATA, sizeof(CLIENT_DATA));
    read(aFD, server_buf, sizeof(server_buf));

    bWritten.get(bytesWritten);		//this call blocks until client thread finishes and returns
    LOG("Client Write: " << CLIENT_DATA);
    ASSERT( bytesWritten == sizeof(CLIENT_DATA) );
    ASSERT( 0 == strcmp(server_buf, CLIENT_DATA) );
    LOG("Server Read: " << server_buf);

    int bytesRead;
    //server writes & client reads
    THREAD_RET_EXECUTE(client, int, bRead, &Peer::read, client_buf, sizeof(client_buf));
    write(aFD, SERVER_DATA, sizeof(SERVER_DATA));
    bRead.get(bytesRead);		//this call blocks until client thread finishes and returns
    LOG("Server Write: " << SERVER_DATA);
    ASSERT( bytesRead == sizeof(SERVER_DATA) );
    LOG("Client Read: " << client_buf);
    ASSERT( 0 == strcmp(client_buf, SERVER_DATA) );

    THREAD_EXECUTE(client, &Peer::close);
    client.exit();

    close(aFD);
    close(sock) ;
    return 0;
}
