#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEVICE 		"/dev/hello"
#define PCI_INFO 	"/usr/share/misc/pci.ids"	
/*
1) 	Коментарий. Строка начинается с символа # и содержит текстовый коментарий к содержанию файла. Пример:
# Vendors, devices and subsystems. Please keep sorted.
2) 	Пустая строка. Используется для разделения блоков коментариев в начале файла.
3) 	Описание производителя.
	Строка начинается с четырехзначного 16-ричного числа, определяющего идентификатор производителя устройства,
	затем два пробела, 
	затем название производителя (может включать пробелы). 
	Пример:
0e11  Compaq Computer Corporation
4) 	Описание устройства. 
	Строка начинается с одного символа табуляции,
	затем четырехзначное 16-ричное число, определяющее идентификатор устройства,
	затем два пробела,
	затем название устройства (может включать пробелы). 
	Пример:
	0046  Smart Array 64xx
5) 	Описание подсистемы устройства.
	Строка начинается с двух символов табуляции,
	затем четырехзначное 16-ричное число, определяющее идентификатор производителя подсистемы устройства,
	затем пробел,
	затем четырехзначное 16-ричное число, определяющее идентификатор подсистемы устройства,
	затем два пробела,
	затем название подсистемы устройства (может включать пробелы). 
	Пример:
		0e11 4091  Smart Array 6i
	Строки описания устройств относятся к ближайшему производителю,
	описание которого встретилось в тексте файла перед ними.
	Строки описания подсистем относятся к ближайшему устройству,
	описание которого встретилось в тексте файла перед ними.
 */
#define BUF_LEN 100
#define STR_LEN 100

void add_zeros(char** str) {
    char* str1 = (char*)calloc(4, sizeof(char));
    if (strlen(*str) == 3) {
        strcpy(str1, *str);
        sprintf(*str, "%s%s","0", str1);
    }
    else if (strlen(*str) == 2) {
        strcpy(str1, *str);
        sprintf(*str, "%s%s","00", str1);
    }
    else if (strlen(*str) == 1) {
        strcpy(str1, *str);
        sprintf(*str, "%s%s","000", str1);
    }
}

int main(int argc, char* argv[]) {
    FILE* hello_dev	= fopen(DEVICE, 	"r");
    FILE* all_pci	= fopen(PCI_INFO,	"r");
    char info_from_dev[STR_LEN];
    int i;
    char all_pci_info[STR_LEN][BUF_LEN];
	char* name;
    char* temp_str;
    char str[STR_LEN];
    unsigned int num1;
    unsigned int num2;

    i = 0;
    temp_str = calloc(STR_LEN, sizeof(char));

    if (hello_dev == NULL) {
        printf("Ошибка открытия файла устройства\n");
        return 0;
    }
    else {
        while (i < BUF_LEN) {
            if (fgets(info_from_dev, BUF_LEN, hello_dev) != NULL)
                strcpy(all_pci_info[i++], info_from_dev);
            else
                break;
        }
        fclose (hello_dev);
    }

    if (all_pci == NULL)
        printf("Ошибка открытия файла с информацией о PCI\n");
    else {
        for (i = 0; i < BUF_LEN; i++) {
            char vend[5];
            char dev[5];
            char subsys[11];

            // выводим номер шины
            name = strtok(all_pci_info[i], " ");
            if (name == NULL)  // если перебрали все строки
                break;
            printf("%s ", name);

            // ищем информацию о вендоре
            name = strtok(NULL, " ");
            strcpy(temp_str, name);
            add_zeros(&temp_str);
            strcpy(vend, temp_str); 
            while (fgets(info_from_dev, STR_LEN, all_pci) != NULL)
                if (!strncmp(temp_str, info_from_dev, 4)) {
                    sscanf(info_from_dev, "%x %100[^\n]", &num1, str);
                    printf("%s ", str);
                    break;
                }
            rewind(all_pci);  // переходим в начало файла

            // ищем информацию об устройстве
            name = strtok(NULL, " ");
            strcpy(temp_str, name);

            add_zeros(&temp_str);
            sprintf(dev, "\t%s", temp_str);    
            while (fgets(info_from_dev, STR_LEN, all_pci) != NULL)
                if (!strncmp(dev, info_from_dev, 5)) {
                    sscanf(info_from_dev, "%x %100[^\n]", &num1, str);
                    printf("%s ", str);
                    break;
                }  
            rewind(all_pci);  // переходим в начало файла

            // ищем информацию о подсистеме
            name = strtok(NULL, " ");
            strcpy(temp_str, name);

            add_zeros(&temp_str);
            sprintf(subsys, "\t\t%s %s", vend, temp_str);                                
            while (fgets(info_from_dev, STR_LEN, all_pci))            
                if (!strncmp(subsys, info_from_dev, 11)) {
                    sscanf(info_from_dev, "%x%x %100[^\n]", &num1, &num2, str);
                    printf("%s ", str);                    
                    break;
                }              
            printf("\n");
            rewind(all_pci);  // переходим в начало файла
        }
        fclose (all_pci);
    }
    free(temp_str);
    return 0;
}

