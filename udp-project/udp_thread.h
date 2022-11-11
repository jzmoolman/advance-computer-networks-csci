#ifndef __UPD_THREAD__
#define __UPD_THREAD__

#include <netinet/in.h> /*  sockaddr_in */

struct thread_args {
	int s;  /* socket fid */
	struct sockaddr_in addr;  
};

#endif
