#ifndef _SOCKET_H_
#define _SOCKET_H_

int socketCreate(void);
int socketConnect(	int hSocket, const char* name, unsigned int serverPort);
int socketSend(		int hSocket, const char* rqst, short lenRqst);
int socketReceive(	int hSocket, 	   char*  rsp, short rvcSize);
void closeSocket(	int hSocket);

#endif // _SOCKET_H_
