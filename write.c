#include <windows.h>
#include "struct.h"

int MyTextOut1(HDC hdc, data_t * data, pos_t * pos, rsize_t * rSize, TEXTMETRIC * tm) {
    int i;
    int j = 0;
    int rows = rSize->height; //количество строк в окне

    line_t * start = data->firstLine;
    for (i = 0; i < pos->v; ++i) {
        start = start->next;
    }

    pos->abs = start->start;

    for (i = pos->v; i - pos->v < rows && i < data->linesSize; ++i) {
        TextOut(hdc, 0, j * tm->tmHeight, start->start + pos->h, fmax(0, start->size - pos->h));
        start = start->next;
        ++j;
    }
}

int PrintLine(HDC hdc, line_t * line, TEXTMETRIC * tm, int v, int h) {
    if (h + 1 > line->size) {
        return;
    }
    int wnd_h_pos = 0;
    block_t * block = line->first;
    int blockPos = 0;
    int totalSize = 0;
        while (h + 1 > totalSize + block->size) {
        totalSize += block->size;
        block = block->next;
    }

    blockPos = h - totalSize;


    TextOut(hdc, 0, v * tm->tmHeight, block->start + blockPos, totalSize + block->size - h);
    wnd_h_pos += block->size - blockPos;
    block = block->next;

    while (block != NULL) {
        TextOut(hdc, wnd_h_pos * tm->tmAveCharWidth, v * tm->tmHeight, block->start, block->size);
        wnd_h_pos += block->size;
        block = block->next;

    }
}

int PrintLine1(HDC hdc, line_t * line, TEXTMETRIC * tm, int v, int h) {
    int wnd_h_pos = 0;
    block_t * block = line->first;

    while (block != NULL) {
        TextOut(hdc, wnd_h_pos * tm->tmAveCharWidth, v * tm->tmHeight, block->start, block->size);
        wnd_h_pos += block->size;
        block = block->next;
    }
}

int MyTextOut(HDC hdc, data_t * data, pos_t * pos, rsize_t * rSize, TEXTMETRIC * tm) {
    int i;
    int j = 0;
    int rows = rSize->height; //количество строк в окне

    line_t * start = data->firstLine;
    pos->abs = start->start;//TODO to delete
    for (i = 0; i < pos->v; ++i) {
        start = start->next;
    }

    for (i = pos->v; i - pos->v < rows && i < data->linesSize; ++i) {
        PrintLine(hdc, start, tm, j, pos->h);
        start = start->next;
        ++j;
    }
}

