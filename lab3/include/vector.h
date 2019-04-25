#ifndef _VECTOR_H_
#define _VECTOR_H_

struct Vector_t {
	char* 			data;
	unsigned int 	size;
};
typedef struct Vector_t Vector_t;

Vector_t createVec(unsigned int size);
Vector_t initVec();
void deleteVec(Vector_t* vec);
void insertVec(Vector_t* dest, Vector_t* src);

#endif // _VECTOR_H_
