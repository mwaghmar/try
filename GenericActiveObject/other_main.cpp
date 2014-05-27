/** 
 * @file other_main.cpp
 * @brief Implementation of test cases without active object pattern
 * @author Manojkumar M Waghmare
 * @version 1.0
 * @date 2011-04-19
 */
#include <unistd.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include "Peer.h"


const int INVALID_SOCKET = -1;
const unsigned PORT_NUM = 12122;
const char DATA[] = { "DEADBEEF" };
const char SERVER_DATA[] = { "IamServer" };
const char CLIENT_DATA[] = { "IamClient" };

void getLocalSockAddr(const int portNum, struct sockaddr_in& saddr);

int testServerRead();
int testServerWrite();
int testAlternateReadWrite();

int main()
{
    testServerRead();
    testServerWrite();
    testAlternateReadWrite();
    std::cerr << "All Tests Executed. Done!!!" << std::endl;
}

/** 
 * @brief Complementary function for testServerRead
 * 
 * @param serv_addr: address of the server to connect to
 * 
 * @return number of bytes written.
 */
void* test_server_read(void* serv_addr)
{
    Peer client;
    struct sockaddr* addr = (struct sockaddr*) serv_addr;
    if( NULL == addr ) {
	return 0;
    }

    client.connect(addr);
    int bytesWritten = client.write(DATA, sizeof(DATA));
    client.close();
    return (void*) bytesWritten;
}

/** 
 * @brief  Test server read operation.
 * 
 * @return 0 if success, negative values otherwise.
 */
int testServerRead()
{
    struct sockaddr_in addr;

    getLocalSockAddr(PORT_NUM, addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if( sock == INVALID_SOCKET ) {
	std::cerr << "socket() failed with errno: " << errno << std::endl;
	return -1;
    }

    int  optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    socklen_t sockln = sizeof(addr);
    if( -1 == bind(sock, (struct sockaddr *) &addr, sockln) ) {
	std::cerr << "bind() failed with errno: " << errno << std::endl;
	return -1;
    }

    listen ( sock, 5 );

    pthread_t tid;
    if( pthread_create(&tid, NULL, test_server_read, &addr) ) {
	std::cerr << "Failed to create client thread" << std::endl;
	return -1;
    }

    int aFD = -1;
    aFD = accept(sock, (struct sockaddr*)&addr, &sockln);
    if( -1 == aFD ) {
	std::cerr << "accept() failed with errno: " << errno << std::endl;
    }

    char Buf[24]= {0};
    read(aFD, Buf, sizeof(Buf));
    if( 0 != strcmp(Buf, DATA)  ) {
	std::cerr << __FILE__ << ": " << __LINE__ << " Failed Assertion: " 
	    << "0 == strcmp(Buf, DATA)" << std::endl;
    }

    close(aFD);

    int bytesWritten;
    if( pthread_join(tid, reinterpret_cast<void**>(&bytesWritten)) ) {
	std::cerr<< "Join Failed" << std::endl;	
    }

    if( bytesWritten != sizeof(DATA) ) {
	std::cerr << __FILE__ << ": " << __LINE__ << " Failed Assertion: " 
	    << "bytesWritten == sizeof(DATA)" << std::endl;
    }

    close(sock);
    return 0;
}

/** 
 * @brief Complementary function for testServerWrite
 * 
 * @param serv_addr: address of the server to connect to
 * 
 * @return number of bytes read.
 */
void* test_server_write(void* serv_addr)
{
    Peer client;
    struct sockaddr* addr = (struct sockaddr*) serv_addr;
    if( NULL == addr ) {
	return 0;
    }

    client.connect(addr);
    char buffer[32] = { 0 };
    int bytesRead = client.read(buffer, sizeof(buffer));

    if( strcmp(buffer, DATA) ) {
	std::cerr << __FILE__ << ": " << __LINE__ << " Failed Assertion: " 
	    << "0 == strcmp(buffer, DATA)" << std::endl;
    }

    client.close();
    return (void*) bytesRead;
}

/** 
 * @brief  Test server Write operation.
 * 
 * @return 0 if success, negative values otherwise.
 */
int testServerWrite()
{
    struct sockaddr_in addr;

    getLocalSockAddr(PORT_NUM, addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if( sock == INVALID_SOCKET ) {
	std::cerr << "socket() failed with errno: " << errno << std::endl;
	return -1;
    }

    int  optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    socklen_t sockln = sizeof(addr);
    if( -1 == bind(sock, (struct sockaddr *) &addr, sockln) ) {
	std::cerr << "bind() failed with errno: " << errno << std::endl;
	return -1;
    }

    listen ( sock, 5 );

    pthread_t tid;
    if( pthread_create(&tid, NULL, test_server_write, &addr) ) {
	std::cerr << "Failed to create client thread" << std::endl;
	return -1;
    }

    int aFD = -1;
    aFD = accept(sock, (struct sockaddr*)&addr, &sockln);
    if( -1 == aFD ) {
	std::cerr << "accept() failed with errno: " << errno << std::endl;
    }

    const int bytesWritten = write(aFD, DATA, sizeof(DATA));
    if( sizeof(DATA) != bytesWritten ) {
	std::cerr << __FILE__ << ": " << __LINE__ << " Failed Assertion: " 
	    << "sizeof(DATA) == bytesWritten" << std::endl;
    }

    close(aFD);

    int bytesRead;
    if( pthread_join(tid, reinterpret_cast<void**>(&bytesRead)) ) {
	std::cerr<< "Join Failed" << std::endl;	
    }

    if( bytesRead != sizeof(DATA) ) {
	std::cerr << __FILE__ << ": " << __LINE__ << " Failed Assertion: " 
	    << "bytesRead == sizeof(DATA)" << std::endl;
    }

    close(sock);
    return 0;
}

/** 
 * @brief Complementary function for testAlternateReadWrite
 * 
 * @param serv_addr: address of the server to connect to
 * 
 * @return NULL
 */
void* test_alternate_read_write(void* serv_addr)
{
    Peer client;
    struct sockaddr* addr = (struct sockaddr*) serv_addr;
    if( NULL == addr ) {
	return 0;
    }

    client.connect(addr);

    const int bytesWritten = client.write(CLIENT_DATA, sizeof(CLIENT_DATA));
    if( sizeof(CLIENT_DATA) != bytesWritten ) {
	std::cerr << __FILE__ << ": " << __LINE__ << " Failed Assertion: " 
	    << "sizeof(CLIENT_DATA) == bytesWritten" << std::endl;
    }

    char client_buf[32] = { 0 };
    client.read(client_buf, sizeof(client_buf));

    if( strcmp(client_buf, SERVER_DATA) ) {
	std::cerr << __FILE__ << ": " << __LINE__ << " Failed Assertion: " 
	    << "0 == strcmp(client_buf, SERVER_DATA)" << std::endl;
    }

    client.close();
    return NULL;
}

/** 
 * @brief Test consecutive read and write operation.
 *  
 * @return 0 on success, negative values otherwise
 **/
int testAlternateReadWrite()
{
    struct sockaddr_in addr;

    getLocalSockAddr(PORT_NUM, addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if( sock == INVALID_SOCKET ) {
	std::cerr << "socket() failed with errno: " << errno << std::endl;
	return -1;
    }

    int  optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    socklen_t sockln = sizeof(addr);
    if( -1 == bind(sock, (struct sockaddr *) &addr, sockln) ) {
	std::cerr << "bind() failed with errno: " << errno << std::endl;
	return -1;
    }

    listen ( sock, 5 );

    pthread_t tid;
    if( pthread_create(&tid, NULL, test_alternate_read_write, &addr) ) {
	std::cerr << "Failed to create client thread" << std::endl;
	return -1;
    }

    int aFD = -1;
    aFD = accept(sock, (struct sockaddr*)&addr, &sockln);
    if( -1 == aFD ) {
	std::cerr << "accept() failed with errno: " << errno << std::endl;
    }

    char server_buf[24]= {0};
    read(aFD, server_buf, sizeof(server_buf));
    if( 0 != strcmp(server_buf, CLIENT_DATA) ) {
	std::cerr << __FILE__ << ": " << __LINE__ << " Failed Assertion: " 
	    << "0 == strcmp(server_buf, CLIENT_DATA)" << std::endl;
    }

    int bytesWritten = write(aFD, SERVER_DATA, sizeof(SERVER_DATA));
    if( sizeof(SERVER_DATA) != bytesWritten ) {
	std::cerr << __FILE__ << ": " << __LINE__ << " Failed Assertion: " 
	    << "sizeof(SERVER_DATA) != bytesWritten" << std::endl;
    }

    if( pthread_join(tid, NULL) ) {
	std::cerr<< "Join Failed" << std::endl;	
    }

    close(aFD);
    close(sock);
    return 0;
}

void getLocalSockAddr(const int portNum, struct sockaddr_in& saddr) 
{
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(portNum);
}
