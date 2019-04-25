
#include "socket.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct sockaddr_in 	Sockaddr_in_t;
typedef struct sockaddr 	Sockaddr_t;
typedef struct hostent 		Hostent_t;
typedef struct in_addr 		In_addr_t;

/*
 *	Создание сокета
 */
int socketCreate(void) {
	int hSocket = socket(AF_INET, SOCK_STREAM, 0);
	return hSocket;
}

/*
 *	Соединение с сервером
 */
int socketConnect(int hSocket, const char* name, unsigned int serverPort) {
	Hostent_t* ht = gethostbyname(name);
	if (ht == NULL) {
		return -1;
	}
	char* address = inet_ntoa(*((In_addr_t*)ht->h_addr_list[0]));
	Sockaddr_in_t remote = {0};
	remote.sin_addr.s_addr 	= inet_addr(address);
	remote.sin_family 		= AF_INET;
	remote.sin_port 		= htons(serverPort);
	int retval = connect(hSocket, (Sockaddr_t*)&remote, sizeof(Sockaddr_in_t));
	return retval;
}

/*
 *	Отправка данных
 */
int socketSend(int hSocket, const char* rqst, short lenRqst) {
	int retval = send(hSocket, rqst, lenRqst, 0);
	return retval;
}

/*
 *	Получение данных
 */
int socketReceive(int hSocket, char* rsp, short rvcSize) {
	int retval = recv(hSocket, rsp, rvcSize, 0);
	return retval;
}

/*
 *	Закрытие сокета
 */
void closeSocket(int hSocket) {
	close(hSocket);
}
