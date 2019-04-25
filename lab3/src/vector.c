
#include "vector.h"

#include <stdlib.h>
#include <string.h>

/* Создание вектора */
Vector_t createVec(unsigned int size) {
	Vector_t vec;
	vec.data = (char*)malloc(size + 1);
	vec.size = size;
	return vec;
}

/* Инициализация вектора */
Vector_t initVec() {
	Vector_t vec;
	vec.data = NULL;
	vec.size = 0;
	return vec;
}

/* Добавление одного вектора в конец другого */
void insertVec(Vector_t* dest, Vector_t* src) {
	char* newData = (char*)malloc(src->size + dest->size + 1);
	memcpy(newData, dest->data, dest->size);
	memcpy(newData + dest->size, src->data, src->size);
	dest->size += src->size;
	free(dest->data);
	dest->data = newData;
}

/* Удаление вектора */
void deleteVec(Vector_t* vec) {
	free(vec->data);
}