#ifndef _CIRCULAR_BUFF_H
#define _CIRCULAR_BUFF_H

#include <linux/types.h>
#include <linux/string.h>

#define DATA_LENGTH 100
#define DATA_SIZE 	3

typedef struct circularBuff {
	size_t head;
	size_t tail;
	char data[DATA_SIZE][DATA_LENGTH];
}	circularBuff_t;

void initCircularBuf(circularBuff_t* buff) {
	buff -> head = 0;
	buff -> tail = 0;
}

unsigned short isEmptyBuff(circularBuff_t* buff) {
	return (buff -> head == buff -> tail);
}
void popBuff(circularBuff_t* buff, char* argData) {
	if (isEmptyBuff(buff)) {
		return;
	}
	strcpy(argData, buff -> data[buff -> head]);
	buff -> head = (buff -> head + 1) % DATA_SIZE;
}
pushBuff(circularBuff_t* buff, char* argData) {
	strcpy(buff -> data[buff -> tail], argData);
	buff -> tail = (buff -> tail + 1) % DATA_SIZE;
	if ( buff -> tail == buff -> head ) {
		buff -> head = (buff -> head + 1) % DATA_SIZE;
	}
}
#endif // _CIRCULAR_BUFF_H

