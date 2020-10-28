#include "struct.h"
#include <assert.h>

void MergeBlocks(block_t * block1, block_t * block2) {
    assert(block1->next == block2);

    if (block1 != NULL && block2 != NULL) {
        if (block2->start == block1->start + block1->size) {
            block1->size += block2->size;
            block1->next = block2->next;
            if (block2->next != NULL) {
                block2->next->prev = block1;
            }
            free(block2);
        }
    }
}

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

void RecountLayout(HWND hwnd, data_t * data, rsize_t * rSize) {

    line_t * line = data->firstLine;
    line_t * newLine;
    line_t * deletedLine;
    block_t * block;
    block_t * prevBlock;
    block_t * lastBlock;
    block_t * newBlock;

    int blockPos;
    int restSize;
    int i = 0;

    int lineCurSize = 0;

    while (line->next != NULL) {
        lineCurSize = 0;
        line->number = i;

        block = line->first;
        // получение последнего возможного блока
        while (block != NULL && lineCurSize + block->size < rSize->width) {
            lineCurSize += block->size;
            prevBlock = block;
            block = block->next;
        }
        if (block == NULL) {
            if (line->next->isGlobal) {
                line = line->next;
            } else {
                // случай, когда мы увеличиваем размер окна
                prevBlock->next = line->next->first;
                line->next->first->prev = prevBlock;

                block = line->next->first;

                if (block != NULL) {
                    MergeBlocks(prevBlock, block);
                    block = prevBlock;
                }

                while (block != NULL && lineCurSize + block->size <= rSize->width) {
                    lineCurSize += block->size;
                    block = block->next;
                }

                if (block == NULL) {
                    deletedLine = line->next;

                    line->next = deletedLine->next;
                    if (deletedLine->next != NULL) {
                        deletedLine->next->prev = line;
                    }
                    line->size += deletedLine->size;

                    free(deletedLine);

                    line = line->next;

                } else {
                    //создаем новый блок, если последний возможный не влезает
                    blockPos = rSize->width - lineCurSize;
                    newBlock = malloc(sizeof(block_t));

                    newBlock->start = block->start + blockPos;
                    newBlock->prev = NULL;
                    newBlock->size = block->size - blockPos;
                    newBlock->next = block->next;

                    // Приводим в порядок исходные данные
                    restSize = line->size + line->next->size - rSize->width;
                    line->size = rSize->width;
                    block->size = blockPos;
                    block->next = NULL;

                    line->next->size = restSize;
                    line->next->first = newBlock;
                    line->next->start = newBlock->start;

                    line = line->next;
                }
            }

        } else {
            //создаем новый блок, если последний возможный не влезает
            blockPos = rSize->width - lineCurSize;
            newBlock = malloc(sizeof(block_t));

            newBlock->start = block->start + blockPos;
            newBlock->prev = NULL;
            newBlock->size = block->size - blockPos;
            newBlock->next = block->next;

            // Приводим в порядок исходные данные
            restSize = line->size - rSize->width;
            line->size = rSize->width;
            block->size = blockPos;
            block->next = NULL;

            MergeBlocks(newBlock, newBlock->next);


            // Теперь переносим нашу цепочку блоков с поправкой на то, является ли строка новой в режиме DEFAULT
            if (line->next->isGlobal) {
                newLine = malloc(sizeof(line_t));

                newLine->next = line->next;
                line->next = newLine;
                newLine->isGlobal = 0;
                newLine->first = newBlock;
                newLine->start = newBlock->start;
                newLine->size = restSize;

                newLine->prev = line;
                line->next = newLine;

                line = line->next;
            } else {
                lastBlock = newBlock;
                while (lastBlock->next != NULL) {
                    lastBlock = lastBlock->next;
                }
                line->next->first->prev = lastBlock;
                lastBlock->next = line->next->first;
                line->next->first = newBlock;
                line->next->start = newBlock->start;
                line->next->size += restSize;

                line = line->next;
            }
        }

        ++i;
    }
    line->number = i;

    block = line->first;
    lineCurSize = 0;

    while (line != NULL) {
        ++i;
        while (block != NULL && lineCurSize + block->size <= rSize->width) {
            lineCurSize += block->size;
            block = block->next;
        }
        if (block == NULL) {
            break;
        }
        //создаем новый блок, если последний возможный не влезает
        blockPos = rSize->width - lineCurSize;
        newBlock = malloc(sizeof(block_t));

        newBlock->start = block->start + blockPos;
        newBlock->prev = NULL;
        newBlock->size = block->size - blockPos;
        newBlock->next = block->next;

        // Приводим в порядок исходные данные
        restSize = line->size - rSize->width;
        line->size = rSize->width;
        block->size = blockPos;
        block->next = NULL;

        // Теперь переносим нашу цепочку блоков
        newLine = malloc(sizeof(line_t));

        newLine->next = line->next;
        line->next = newLine;
        newLine->isGlobal = 0;
        newLine->first = newBlock;
        newLine->start = newBlock->start;
        newLine->size = restSize;

        newLine->prev = line;
        line->next = newLine;

        line = line->next;
    }

    data->linesSize = i;
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

