/** 
 * @file Peer.cpp
 * @brief Implementation of class Peer for socket communication.
 * @author Manojkumar M Waghmare
 * @version 1.0
 * @date 2011-04-11
 */
#include "ace/OS.h"
#include "Peer.h"
//#include <iostream>

Peer::Peer(): sock_(-1)
{
	sock_ = socket(AF_INET, SOCK_STREAM, 0);
}

void Peer::connect(sockaddr* addr)
{
	bool gotConnected=false;
	while ( !gotConnected ) {
		int val  = ::connect( sock_, addr, sizeof (sockaddr_in));
		if(val != -1){
			gotConnected=true;
		}
	}	

	//std::cout<< "#Client: Got Connected successgully"<< std::endl;  
}

int Peer::read(char* buf, unsigned int buflen)
{
	if( NULL == buf ) {
		return -1;
	}

	int bytesRead = ::read(sock_, (void*)buf, buflen);
	return bytesRead;
}

int Peer::write(const char* buf, int bufLen)
{
	const int byteWritten = ::write(sock_,(void*)buf,bufLen);
	return byteWritten;
}

void Peer::close()
{
	::close(sock_);
}

