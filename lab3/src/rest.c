/*
 *	
    Через аргументы командной строки получает аргументы вызова Web API http://www.iu3.bmstu.ru/WebApi/time 
	а также тип HTTP запроса, который будет использоваться для вызова API (GET или POST)
    Формирует строку запроса
    Соединяется c сервером www.iu3.bmstu.ru через 80-й порт
    Пересылает запрос
    Получает ответ
    Проверяет код ошибки в первой строке ответа
    Если код ошибки не равен 200 печатает этот код и завершает работу
    Иначе, печатает данные из HTTP ответа сервера
 *	--
	Сокет это комбинация IP адреса компьютера и номера порта, 
	которая позволяет идентифицировать в сети программу, передающую или принимающую данные
 *	--
 	Веб-сокетам же для ответа не нужны ваши повторяющиеся запросы. 
	Достаточно выполнить один запрос и ждать отклика. 
	Вы можете просто слушать сервер, который будет отправлять вам сообщения по мере готовности.
 *	--
	   Веб-клиент				Веб-сервер
	   прикладной	  HTTP		прикладной
	 транспортный	  TCP		транспортный
		  сетевой	   IP		сетевой
		канальный	Ethernet	канальный
 */
#include "socket.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUEST_SIZE	128	// Максимальный размер запроса
#define MAX_CONTENT 	128	// Максимальный размер тела запроса
#define MAX_PACKET 		128	// Максимальный размер пакета

#define DOMAIN_NAME "www.iu3.bmstu.ru"
#define PORT 		8090
#define PATH 		"/WebApi/time"

/* Конфигурация запроса */
struct Conf_t {
	int 	isPost;
	char* 	type;
	char* 	format;
};	typedef struct Conf_t Conf_t;

int main(int argc, char* argv[]) {

	char* request = (char*)malloc(REQUEST_SIZE);
	char* content = (char*)malloc(MAX_CONTENT);

	Conf_t configuration = {
		.isPost = 0, 
		.format = "internet", 
		.type 	= "utc"
	};

	// Обработка аргуменов командной строки
	if (argc > 1) {
		for (unsigned int i = 1; i < argc; ++i) {
			if (strcmp(argv[i], "-post") == 0) {
				configuration.isPost = 1;
			} else if (strcmp(argv[i], "-t") == 0) {
				configuration.type = argv[i + 1];
			} else if (strcmp(argv[i], "-f") == 0) {
				configuration.format = argv[i + 1];
			}
		}
	}

	// Формирование заголовка и тела запроса
	sprintf(content, "type=%s&format=%s", configuration.type, configuration.format);
	if (configuration.isPost) {
		sprintf(request,
				"POST %s HTTP/1.0\r\n"
				"Content-Type:application/x-www-form-urlencoded\r\n"
				"Content-Length:%zu\r\n"
				"\r\n"
				"%s\n",
				PATH, strlen(content), content);
	} else {
		sprintf(request,
				"GET %s?%s "
				"HTTP/1.0\n\n",
				PATH, content);
	}
	printf("%s\n", request);

	int hSocket = socketCreate();
	if (hSocket <= 0) {
		printf("Error: unable to create socket");
		return 1;
	}

	int res = socketConnect(hSocket, DOMAIN_NAME, PORT);
	if (res != 0) {
		printf("Error: unable to connect to server");
		closeSocket(hSocket);
		return 1;
	}

	res = socketSend(hSocket, request, strlen(request));
	if (res == -1) {
		printf("Error: unable to send data");
		closeSocket(hSocket);
		return 1;
	}

	int rcvSize = 0;
	Vector_t response 	= initVec();
	Vector_t packet 	= createVec(MAX_PACKET);

	// Приём и сохранение результатов запроса
	do {
		rcvSize = socketReceive(hSocket, packet.data, MAX_PACKET);
		if (rcvSize == -1) {
			printf("Error: unable to recieve data");
			closeSocket(hSocket);
			return 1;
		}
		packet.size = rcvSize;
		insertVec(&response, &packet);
	} while (rcvSize == MAX_PACKET);
	deleteVec(&packet);

	response.data[response.size] = '\0';

	// Проверка ответа
	if (strstr(response.data, "HTTP/1.1 200 OK") == NULL) {
		printf("Error: response code isn't 200");
		return 1;
	}
	char* pos = strstr(response.data, "\r\n\r\n");
	printf("%s\n", pos + sizeof("\r\n\r\n") - 1);

	deleteVec(&response);
	free(request);
	free(content);
	closeSocket(hSocket);
	return 1;
}
