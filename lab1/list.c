#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define  MAX_LINE_LENGTH 256
#define  DBNAME "db.csv"

/*  
 *  Тип указателя на функцию для сравнения элементов списка
 */
typedef int (*CMP_FUNC)(COUNTRY* v1, COUNTRY* v2);

/*
 *  Выделяет динамическую память для нового элемента списка list,
 *  инициализирует поля и добавляет элемент в начало списка.
 *  Изменяет указатель на начало списка. 
 *  Возвращает: 
 * 				0, если новый элемент добавлен 
 *         или 	1, если не удалось выделить память для элемента.
 */
int add(COUNTRY** list, char* name, int population, int area) {
    COUNTRY* new_elem = malloc(sizeof(COUNTRY));
    if (new_elem == NULL) {
        return 1;
    }
    strncpy(new_elem -> name, name, sizeof(new_elem -> name));
    new_elem -> population 	= population;
    new_elem -> area 		= area;
    new_elem -> next 		= *list;
    *list = new_elem;
    return 0; /* ok */
}

/*  
 *  Удаляет элемент списка list и освобождает выделенную по него память.
 *  Если элемент первый, изменяет указатель на список.
 *  Если других элементов в списке нет, после его удаления list = NULL
 */
void delete(COUNTRY** list, COUNTRY* v) {
    if (list == NULL || *list == NULL || v == NULL) {
        return;
    }
    if (compare_name(*list, v) == 0) {
        COUNTRY* target = *list;
        if ((*list) -> next != NULL) {
            (*list) -> next = (*list) -> next -> next;
            *list = (*list) -> next;
        } else {
            *list = NULL;
        }
        free(target);
        return;
    }
    for (COUNTRY* it = (*list); it -> next != NULL; it = it -> next) {
        if (compare_name(it -> next, v) == 0) {
            COUNTRY* target = it -> next;
            it -> next 		= it -> next -> next;
            free(target);
            return;
        }
    }
}

/*
 *  Находит элемент в списке по названию страны.
 *  Возвращает указатель на элемент COUNTRY
 *  или NULL, если страна отсутсвует в списке list.
 */
COUNTRY* find(COUNTRY* list, char* name) {
    for (COUNTRY* it = list; it != NULL; it = it -> next) {
        if (strcmp(it -> name, name) == 0) {
            return it;
        }
    }
    return NULL;
}

/*
 * Возвращает количество элементов в списке
 */
int count(COUNTRY* list) {
    int cnt = 0;
    for (COUNTRY* it = list; it != NULL; it = it -> next) {
        ++cnt;
    }
    return cnt;
}

/* 
 *  Рекурсивный алгорит быстрой сортировки,
 *  сортирует массив указетей на элементы COUNTRY. 
 */
void quick_sort(COUNTRY** ca, int first, int last, CMP_FUNC compare) {
    int i, j;
    COUNTRY* v;
    COUNTRY* p;
    i = first;
    j = last;
    v = ca[(first + last) / 2];

    do {
        while (compare(ca[i], v) < 0) i++;
        while (compare(ca[j], v) > 0) j--;

        if (i <= j) {
            if (compare(ca[i], ca[j]) > 0) {
                p 		= ca[j];
                ca[j] 	= ca[i];
                ca[i] 	= p;
            }
            i++;
            j--;
        }
    } while (i <= j);

    if (i < last)
        quick_sort(ca, i, last, compare);

    if (first < j)
        quick_sort(ca, first, j, compare);
}

int compare_name(COUNTRY* v1, COUNTRY* v2) {
    return strcmp(v1 -> name, v2 -> name);
}

int compare_area(COUNTRY* v1, COUNTRY* v2) {
    if (v1 -> area < v2 -> area) {
        return -1;
    } else if (v1 -> area == v2 -> area) {
        return 0;
    }
    return 1;
}

int compare_population(COUNTRY* v1, COUNTRY* v2) {
    if (v1 -> population < v2 -> population) {
        return -1;
    } else if (v1 -> population == v2 -> population) {
        return 0;
    }
    return 1;
}

/*
 *  Функция сортировки элементов списка
 *	Преобразование типов:
 *  (void** list, CMP_FUNC compare) {}
 *  typedef int (*CMP_FUNC)(void*, void*);
 */
int sort(COUNTRY** list, CMP_FUNC compare) {
    /*
     *  Принимаются аргументы:
     *  первый - указатель на начало, который может измениться после сортировки
     *  второй - CMP_FUNC - указатель на функцию сравнения:
     *  Тип указателя CMP_FUNC объявлен выше:   int func(COUNTRY * v1, COUNTRY * v2);
     *  typedef int (*CMP_FUNC)(COUNTRY *v1, COUNTRY *v2);
     *  Возвращает значение:
     *  > 0, if v1 > v2
     *  < 0, if v1 < v2
     *    0, if v1 = v2
     */
    int cnt, i;
    COUNTRY *p, **ca = NULL;

    cnt = count(*list);
    if (cnt < 2)
        return 0;

    /* выделяем память под массив указателей */
    ca = (COUNTRY**) malloc(cnt* sizeof(COUNTRY*));
    if (!ca)	return 1;

    /* заполняем массив указателями на элементы списка */
    ca[0] = *list;
    for (i = 1; i < cnt; i++)
        ca[i] = ca[i - 1]->next;

    quick_sort(ca, 0, cnt - 1, compare);

    /*
	 *	заполняем список элементами из отсортированного массива
	 */
    *list = NULL;
    while (cnt > 0) {
        ca[cnt - 1]->next = *list;
        *list = ca[cnt - 1];
        cnt--;
    }
    free(ca);
    return 0;
}

int sort_by_name(COUNTRY** list) {
    if (list == NULL || *list == NULL) {
        return 1;
    }
    return sort(list, compare_name);
}

int sort_by_area(COUNTRY** list) {
    if (list == NULL || *list == NULL) {
        return 1;
    }
    return sort(list, compare_area);
}

int sort_by_population(COUNTRY** list) {
    if (list == NULL || *list == NULL) {
        return 1;
    }
    return sort(list, compare_population);
}

/*
 *  Выводит информцию о стране
 */
void print_country(COUNTRY* p) {
    if (p == NULL) {
        return;
    }
    if (p -> population == 0 && p -> area == 0) {
        printf("%s, не указано, не указано\n", p -> name);				//	Только название
    } else if (p -> population == 0) {
        printf("%s, не указано, %d\n", p -> name, p -> population);		//	Название и территория
    } else if (p -> area == 0) {
        printf("%s, %d, не указано\n", p -> name, p -> area);			//	Название и население
    } else {
        printf("%s, %d, %d\n", p -> name, p -> population, p -> area);	//	Все параметры страны
    }
}

/*
 *  Функция dump выводит список стран на экран.
 *  Вызывает функцию print_country для вывода информации о каждой стране в списке.
 */
void dump(COUNTRY* list) {
    if (list == NULL) {
        return;
    }
    COUNTRY* p;
    p = list; /* Начало списка */
    while (p != NULL) {
        print_country(p);
        p = p -> next;
    }
}

/*
 *  Удаляет все элементы списка,
 *  освобождает выделенную для них память,
 *  присавивает значение NULL указателю на начало списке.
 */
void clear(COUNTRY* list) {
    COUNTRY* it = list;
    while (it != NULL) {
        COUNTRY* tmp  = it;
        it = it -> next;
        free(tmp);
    }
}

/*
 *  Загружает записи из файла 
 *  и формирует однонаправленный связный список в памяти.
 *  Возвращает указатель на этот список.
 */
COUNTRY* load() {
    char 	buf[MAX_LINE_LENGTH + 1];
    char* 	par[3];
    int cnt, pcount = 0;
    COUNTRY *p, *list = NULL;
    FILE* f = fopen(DBNAME, "r");

    buf[MAX_LINE_LENGTH] = 0x00;

    if (f) {
        while (fgets(buf, MAX_LINE_LENGTH, f)) {
            pcount = 0;
            par[pcount++] = buf;
            cnt = 0;
            while (buf[cnt]) {
                if (buf[cnt] == ',') {
                    buf[cnt] 		= 0x00;
                    par[pcount++] 	= &buf[cnt + 1];
                }
                cnt++;
            }
            if (pcount == 3) {
                add(&list, par[0], atoi(par[1]), atoi(par[2]));
            }
        }
        fclose(f);
    }
    return list;
}

/*
 *	Сохраняет список стран в файл
 */
void save(COUNTRY* list) {
    FILE* f 	= fopen(DBNAME, "w+");
    COUNTRY* p 	= list;

    if (f) {
        while (p != NULL) {
            fprintf(f, "%s,%d,%d\n", p -> name, p -> population, p -> area);
            p = p -> next;
        }
        fclose(f);
    }
}
