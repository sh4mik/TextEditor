#include "struct.h"
#include "stdio.h"

void PrintStr(char * str, int n) {
    int i = 0;
    for ( i = 0; i < n; ++i) {
        printf("%c", str[i]);
    }
}

void PrintBlocks(line_t * line) {
    block_t * block = line->first;
    int i = 0;
    while (block != NULL) {
        printf("%i:|", i);
        PrintStr(block->start, block->size);
        printf("|, ");
        block = block->next;
        ++i;
    }
    printf("\n");
}

void PrintWindow(data_t * data, pos_t * pos, int h) {
    int i;
    line_t * line = data->firstLine;
    for (i = 0; i < pos->v; ++i) {
        line = line->next;
    }

    for (i = 0; i < h && line != NULL; ++i) {
        printf("%i: s=%i, g=%i;\t", line->number, line->size, line->isGlobal);
        PrintBlocks(line);
        line = line->next;
    }
    printf("---------------------------------------------------------------------\n");
}

int PrintGlobal(HDC hdc, data_t * data, pos_t * pos, rsize_t * rSize, TEXTMETRIC * tm) {
    int i;
    int j = 0;
    int rows = rSize->height; //количество строк в окне

    line_t * start = data->firstLine;
    for (i = 0; i < pos->v; ++i) {
        start = start->next;
    }

    pos->abs = start->start;

    for (i = pos->v; i - pos->v < rows && i < data->linesSize; ++i) {
        printf("%i\n", start->isGlobal);
        start = start->next;
    }
}
