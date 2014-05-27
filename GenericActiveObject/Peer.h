/** 
 * @file Peer.h
 * @brief Simple class that implements socket read write functions.
 * @author Manojkumar M Waghmare
 * @version 1.0
 * @date 2011-04-11
 */
#ifndef __PEER_H__
#define __PEER_H__

#include <sys/socket.h>

class Peer 
{
	public:

		Peer();
		void connect(sockaddr* addr);
		int read(char* buf, unsigned int buflen);
		int write(const char* buf,int bufLen);
		void close();

	private:

		int		sock_;
};

#endif /* __PEER_H__ */

