#ifndef STRUCT_H_INCLUDED
#define STRUCT_H_INCLUDED

#include <windows.h>

#define SCROLL_CONST 1000

#define OLD_DATA 0
#define NEW_DATA 1

typedef struct block {
    char * start;   // указатель на начало в буффере данных
    int size;       // размер блока
    int type;       // тип данных(старые или новые) - оказался пока не нужен
    struct block * next;
    struct block * prev;
} block_t;

typedef struct line {
    int number;     // номер в списке строк
    char * start;   // указатель на начало строки
    int size;
    int isGlobal;   // является ли строка новой в действительности (в режиме LAYOUT)
    struct line * next;
    struct line * prev;
    block_t * first;    // первый блок в строке
} line_t;

typedef struct data {
    char * text;
    int textSize;  // размер буфера для исходных данных
    char * buff;    // указатель на начало буфера для дополнительных данных
    char * buffPos; // текущяя позиция для записи в буфер
    int buffSize;  // размер буфера для доп данных
    int linesSize;  // количество строк
    int longestSize;  // размер самой длинной строки
    line_t * firstLine;  // первая строка
} data_t;

typedef struct pos {
    int v; //позиция по вертикали
    int h; // по горизонтали

    char * abs; // номер символа (оказался ненужным)
} pos_t;

// размер окошка в колчестве умещающихся символов
typedef struct size_s {
    int width;
    int height;
} rsize_t;


void FirstCountDefault(HWND hwnd, data_t * data);

void RecountDefault(HWND hwnd, data_t * data);

void FindNearestLines(data_t * data, pos_t * pos);

int PosToScroll(int val, data_t data, pos_t pos, int lines);

int ScrollToPos(int val, data_t data, pos_t pos, int lines);

void freeData(data_t * data);

#endif // STRUCT_HINCLUDED
