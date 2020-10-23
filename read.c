#include <windows.h>
#include <stdio.h>
#include "struct.h"

int FileRead(HWND hwnd, const char * filename, data_t * data) {
    int i;
    int realFileSize;
    int length_of_file;
    char * pos;
    FILE * fp = fopen(filename,"rb");
    if (fp == NULL){
        MessageBox(hwnd,
                   "File could not be opened",
                   "File error",
                   MB_OK); // команда ОК
        return 0;
    }

    fseek(fp,0,SEEK_END); // указатель на объект смещения, кол-во байт смещения, позиция, относительно которой смещение
    length_of_file = ftell(fp); // возвращает текущее положение потока
    rewind(fp); // перемещаем в начало

    // иницализируем память
    realFileSize = length_of_file + 2000;
    data->text = (char*)malloc((realFileSize) * sizeof(char));       // массив символов
                                                                // тогда таких строк будет столько же, сколько и символов
    // если все плохо, чистим память, файл закрываем
    if (data->text == NULL){
        MessageBox(hwnd,
                   "Not enough memory is available",
                   "Memory error",
                   MB_OK);
        fclose(fp);
        free(data->text);
        ZeroMemory(data,sizeof(data)); // заполняем блок памяти(указатель на блок памяти, размер блока памяти)
        return 0;
    }

    fread(data->text,sizeof(char),length_of_file + 1,fp); // считали символы в буфер
    data->text[length_of_file] = '\0'; // нуль-терминирование строки символов (когда файл закончился)
    fclose(fp); //закрытие файла

    data->textSize = length_of_file;
    data->buff = data->text + length_of_file + 1;
    data->buffPos = data->buff;
    data->buffSize = realFileSize - data->textSize;

    return 1;
}
