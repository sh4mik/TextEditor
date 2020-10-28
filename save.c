#include <stdio.h>
#include "save.h"

void SaveFile(HWND hwnd, data_t * data, const char * fileName) {
    int i;
    FILE * fp = fopen(fileName, "w+");
    if (fp == NULL) {
        MessageBox(hwnd, "Cant open file to save", "Sorry!(((", MB_OK);
        return 0;
    }

    line_t * line = data->firstLine;
    block_t * block;
    while (line != NULL) {
        block = line->first;
        while (block != NULL) {
            for (i = 0; i < block->size; ++i) {
                fputc(block->start[i], fp);
            }
            block = block->next;
        }
        line = line->next;
        if (line != NULL) {
            //fputc('\r', fp);
            fputc('\n', fp);
        }
    }

    fclose(fp);
    return 0;
}
