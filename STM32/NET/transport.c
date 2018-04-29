

#if !defined(SOCKET_ERROR)
	/** error in socket operation */
	#define SOCKET_ERROR -1
#endif


#define INVALID_SOCKET SOCKET_ERROR
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/


int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	int rc = 0;
	//rc = write(sock, buf, buflen);
	return rc;
}


int transport_getdata(unsigned char* buf, int count)
{
	
	return 0;
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	
	return 0;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(char* addr, int port)
{
	return 0;
}

int transport_close(int sock)
{
	return 0;
}

