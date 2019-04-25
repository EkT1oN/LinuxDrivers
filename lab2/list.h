#ifndef _LIST_H_
#define _LIST_H_

typedef struct _COUNTRY {
    char	name[256];
    int 	population;
    int 	area;
    struct _COUNTRY* next;
} COUNTRY;


int add(COUNTRY** list, char* name, int population, int area);

int compare_name(COUNTRY* v1, COUNTRY* v2);
int count(				COUNTRY* list);
int sort_by_name(		COUNTRY** list);
int sort_by_area(		COUNTRY** list);
int sort_by_population(	COUNTRY** list);

void dump(			COUNTRY* list);
void save(			COUNTRY* list);
void clear(			COUNTRY* list);
void print_country(	COUNTRY* p);
void delete(COUNTRY** list, COUNTRY* v);

COUNTRY* find(COUNTRY* list, char* name);
COUNTRY* load();

#endif // _LIST_H_
