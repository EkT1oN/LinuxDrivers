#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

int main(int argc, char *argv[]) {
    /*
     *  В начале работы загружается список стран,
     *  Функция загрузки списка load() объявлена в файле list.h
     */
    COUNTRY *list;
    list = load();

    if (argc > 1) {
        //  strcmp - функция сравнения строк
        if (strcmp(argv[1], "add") == 0) {
            if (argc == 5) {
                char *end;
                int population = (int) strtol(argv[3], &end, 10);
                int area = (int) strtol(argv[4], &end, 10);
                add(&list, argv[2], population, area);
            }
        } else if (strcmp(argv[1], "delete") == 0) {
            if (argc == 3) {
                COUNTRY *c = find(list, argv[2]);
                delete(&list, c);
            }
        } else if (strcmp(argv[1], "dump") == 0) {
            if (argc == 2) {
                dump(list);
            }
            if (argc == 3) {
                if (strcmp(argv[2], "-n") == 0) {
                    sort_by_name(&list);
                }
                if (strcmp(argv[2], "-a") == 0) {
                    sort_by_area(&list);
                }
                if (strcmp(argv[2], "-p") == 0) {
                    sort_by_population(&list);
                }
                dump(list);
            }
        } else if (strcmp(argv[1], "view") == 0) {
            if (argc == 3) {
                COUNTRY *c = find(list, argv[2]);
                if (c != NULL) {
                    print_country(c);
                }
            }
        } else if (strcmp(argv[1], "count") == 0) {
            if (argc == 2) {
                printf("%d\n", count(list));
            }
        }
    }
    save(list);
    /* 
     *  Функция clear удаляет элементы из списка 
     *  и освобождает выделенную для них память.
     */
    clear(list);
    return 0;
}
