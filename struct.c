#include "struct.h"

void FirstCountDefault(HWND hwnd, data_t * data) {
    char * pos;

    data->firstLine = malloc(sizeof(line_t));
    data->firstLine->start = data->text;
    data->firstLine->number = 0;
    data->firstLine->first = malloc(sizeof(block_t));
    data->firstLine->first->start = data->text;
    data->firstLine->first->next = NULL;
    data->firstLine->first->prev = NULL;
    data->firstLine->first->type = OLD_DATA;
    data->firstLine->isGlobal = 1;
    data->firstLine->prev = NULL;

    data->linesSize = 1;
    data->longestSize = 0;

    line_t * tmp;
    line_t * prev = data->firstLine;
    // Запись метаданных
    for (pos = data->text; pos < data->text + data->textSize; ++pos) {
        if (*pos == '\n') {
            tmp = malloc(sizeof(line_t));
            tmp->start = pos + 1;
            tmp->first = malloc(sizeof(block_t));
            tmp->first->start = tmp->start;
            tmp->first->next = NULL;
            tmp->first->prev = NULL;
            tmp->first->type = OLD_DATA;
            tmp->number = data->linesSize;
            tmp->isGlobal = 1;
            tmp->prev = prev;

            prev->size = pos - 1 - prev->start;
            prev->first->size = prev->size;
            prev->next = tmp;


            if (data->longestSize < prev->size) {
                data->longestSize = prev->size;
            }

            ++data->linesSize;

            prev = tmp;
            tmp = tmp->next;
        }
    }

    prev->size = pos - prev->start;
    prev->first->size = prev->size;
    if (data->longestSize < prev->size) {
        data->longestSize = prev->size;
    }

    prev->next = NULL;
}

void FirstCountLayout(HWND hwnd, data_t * data, rsize_t * rSize){
    //максимальное количество умещающихся в строку символов
    int max_am = rSize->width;
    char *pos;

    data->firstLine = malloc(sizeof(line_t));
    data->firstLine->start = data->text;
    data->firstLine->isGlobal = 1;
    data->firstLine->number = 0;
    data->firstLine->first = malloc(sizeof(block_t));
    data->firstLine->first->start = data->text;
    data->firstLine->first->next = NULL;
    data->firstLine->first->prev = NULL;
    data->firstLine->first->type = OLD_DATA;
    data->firstLine->prev = NULL;



    data->linesSize = 1;
    data->longestSize = 0;

    line_t * tmp;
    line_t * prev = data->firstLine;

    for (pos = data->text; *pos != 0; ++pos){ //проход по всем символам файла
        if (*pos == '\n' || pos - 1 - prev->start == max_am){
            tmp = malloc(sizeof(line_t));
            if (*pos == '\n') {
                tmp->start = pos + 1;
            } else {
                tmp->start = pos - 1;
            }

            if (*pos == '\n') {
                tmp->isGlobal = 1;
            } else {
                tmp->isGlobal = 0;
            }
            tmp->first = malloc(sizeof(block_t));
            tmp->first->next = NULL;
            tmp->first->prev = NULL;
            tmp->first->type = OLD_DATA;
            tmp->first->start = tmp->start;
            tmp->number = data->linesSize;
            tmp->prev = prev;

            prev->size = pos - 1 - prev->start;
            prev->first->size = prev->size;
            prev->next = tmp;

            ++data->linesSize;

            prev = tmp;
            tmp = tmp->next;
        }
    }

    prev->size = pos - prev->start;
    prev->first->size = prev->size;
    prev->next = NULL;
}

void RecountDefault(HWND hwnd, data_t * data) {

}
void RecountLayout(HWND hwnd, data_t * data, rsize_t * rSize) {

}

void FindNearestLines(data_t * data, pos_t * pos) {
    int i = 0;
    int max = 0;

    line_t * cur = data->firstLine;

    while (pos->abs != cur->start) {
        if (cur->isGlobal == 1) {
            max = i;
        }
        cur = cur->next;
        ++i;
    }

    if (cur->isGlobal != 1)
        pos->v = max;
    else
        pos->v = i;
}


void freeData(data_t * data) {
    line_t * prev;
    line_t * cur = data->firstLine;

    block_t * prevB;
    block_t * curB;

    free(data->text);
    while (cur != NULL) {
        prev = cur;
        cur = cur->next;

        curB = prev->first;
        while(curB != NULL) {
            prevB = curB;
            curB = curB->next;
            free(prevB);
        }
        free(prev);
    }
}

int PosToScroll(int val, data_t data, pos_t pos, int lines) {
    return val * SCROLL_CONST / fmax(0, data.linesSize - lines);
}

int ScrollToPos(int val, data_t data, pos_t pos, int lines) {
    return val * (fmax(0, data.linesSize - lines)) / SCROLL_CONST;
}

