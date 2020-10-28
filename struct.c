#include "struct.h"
#include <assert.h>

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

void RecountDefault(HWND hwnd, data_t * data) {
    line_t * deletedLine = data->firstLine->next;
    line_t * prevLine;
    block_t * block;
    block_t * nlBlock;

    data->longestSize = 0;
    while (deletedLine != NULL) {
        // если новая строка в режиме без верстки
        if (deletedLine->isGlobal) {
            deletedLine = deletedLine->next;
        } else {
            // ищем последний блок в строке
            prevLine = deletedLine->prev;

            block = prevLine->first;
            while (block->next != NULL) {
                block = block->next;
            }

            //соединим строки
            prevLine->size += deletedLine->size;
            prevLine->next = deletedLine->next;
            if (deletedLine->next != NULL) {
                deletedLine->next->prev = prevLine;
            }

            //теперь соединим блоки
            block->next = deletedLine->first;
            deletedLine->first->prev = block;

            // склеиваем блоки, если это возможно
            if (block->next->start == block->start + block->size) {
                nlBlock = block->next;
                block->size += nlBlock->size;
                block->next = nlBlock->next;
                if (nlBlock->next != NULL) {
                    nlBlock->next->prev = block;
                }
                free(nlBlock);
            }

            // и удалим строку
            free(deletedLine);
            deletedLine = prevLine->next;

            //уменьшим количество строк и обработаем максимально длинную строку
            data->linesSize--;

            if (prevLine->size > data->longestSize) {
                data->longestSize = prevLine->size;
            }
        }
    }
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

