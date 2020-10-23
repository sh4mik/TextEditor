#include "edit.h"

int isOld(block_t * block, data_t * data) {
    if (block->start < data->text + data->textSize) {
        return 1;
    } else {
        return 0;
    }
}

void AddSym(data_t * data, caret_t * caret, pos_t * pos, int mode, char ch) {
    int blockPos = 0;
    int totalSize = 0;

    block_t * block2;
    block_t * block3;

    block_t * block = caret->line->first;

    if (mode == DEFAULT) {

        while (caret->sym + 0 > totalSize + block->size) {
            totalSize += block->size;
            block = block->next;
        }
        blockPos = caret->sym - totalSize;

        // если добавл€ем в конец блока
        if (blockPos == block->size) {
            // если конец блока совпадает с концом позиции буфера (дл€ последовательного воода в позицию)
            if (block->start + block->size == data->buffPos) {
                block->size++;
                *data->buffPos = ch;
                ++data->buffPos;
                ++caret->line->size;
            } else {
                block3 = block->next;
                block2 = malloc(sizeof(block_t));

                block->next = block2;
                block2->prev = block;
                block2->next = block3;
                block2->size = 1;
                block2->start = data->buffPos;
                *data->buffPos = ch;
                ++data->buffPos;
                ++caret->line->size;

                if (block3 != NULL) {
                    block3->prev = block2;
                }

                if (block->size == 0 && caret->line->first == block) {
                    caret->line->first = block2;
                    block2->prev = caret->line->first;
                    free(block);
                }

            }
            //++caret->sym;
            return;
        }

        // если символ первый в блоке
        if (blockPos == 0) {
            block3 = block->prev;
            block2 = malloc(sizeof(block_t));

            block2->size = 1;
            block2->start = data->buffPos;
            *data->buffPos = ch;
            data->buffPos++;
            block2->next = block;
            block->prev = block2;
            block2->prev = block3;
            ++caret->line->size;


            if (block3 != NULL) {
                block3->next = block2;
            } else {
                caret->line->first = block2;
            }
            //++caret->sym;
            return;
        }

        // если символ не первый и не последний
        block3 = malloc(sizeof(block_t));
        block2 = malloc(sizeof(block_t));

        block3->start = block->start + blockPos;
        block3->size = block->size - blockPos;
        block3->next = block->next;
        block3->prev = block2;

        block->size = blockPos;
        block->next = block2;

        block2->prev = block;
        block2->next = block3;
        block2->size = 1;
        block2->start = data->buffPos;
        *data->buffPos = ch;
        data->buffPos++;

        caret->line->size++;

        //++caret->sym;
    }
}


void AddEnter(data_t * data, caret_t * caret, pos_t * pos, int mode, char ch) {
    int blockPos = 0;
    int totalSize = 0;

    block_t * block2;
    block_t * block3;

    block_t * block = caret->line->first;
    line_t * newLine = malloc(sizeof(line_t));
    newLine->isGlobal = 1;
    if (mode == DEFAULT) {

        while (caret->sym + 0 > totalSize + block->size) {
            totalSize += block->size;
            block = block->next;
        }
        blockPos = caret->sym - totalSize;

        // если добавл€ем в конец блока
        if (blockPos == block->size) {

            // если мы в конце строки
            if (block->next == NULL) {
                block2 = malloc(sizeof(block_t));

                block2->prev = NULL;
                block2->next = NULL;
                block2->size = 0;
                block2->start = data->buffPos;
                *data->buffPos = ch;
                ++data->buffPos;

                newLine->size = 0;
            } else {
                block2 = block->next;
                block2->prev = NULL;
                block->next = NULL;

                newLine->size = caret->line->size - caret->sym;
                caret->line->size = caret->sym;
            }

            newLine->prev = caret->line;
            newLine->next = caret->line->next;
            newLine->prev->next = newLine;
            if (newLine->next != NULL){
                newLine->next->prev = newLine;
            }

            newLine->first = block2;
            newLine->start = block2->start;
            newLine->number = newLine->prev->number;

            data->linesSize++;
            caret->line = caret->line->next;

            while (newLine != NULL) {
                newLine->number++;
                newLine = newLine->next;
            }



            //++caret->sym;
            return;
        }

        // если символ первый в блоке
        if (blockPos == 0) {

            block_t * block2 = malloc(sizeof(block_t));

            block2->prev = NULL;
            block2->next = NULL;
            block2->size = 0;
            block2->start = data->buffPos;
            *data->buffPos = ch;
            ++data->buffPos;

            newLine->prev = caret->line->prev;
            newLine->next = caret->line;
            newLine->next->prev = newLine;

            newLine->size = 0;
            newLine->first = block2;
            newLine->start = block2->start;

            if (newLine->prev != NULL) {
                newLine->number = newLine->prev->number + 1;
                newLine->prev->next = newLine;
            } else {
                data->firstLine = newLine;
                newLine->number = 0;
            }

            data->linesSize++;

            newLine = newLine->next;
            while (newLine != NULL) {
                newLine->number++;
                newLine = newLine->next;
            }

            //++caret->sym;
            return;
        }

        // если символ не первый и не последний
        block2 = malloc(sizeof(block_t));

        block2->prev = NULL;
        block2->next = block->next;
        block2->size = block->size - blockPos;
        block2->start = block->start + blockPos;


        newLine->prev = caret->line;
        newLine->next = caret->line->next;
        if (newLine->next != NULL) {
                newLine->next->prev = newLine;
        }

        newLine->size = caret->line->size - caret->sym;
        newLine->first = block2;
        newLine->start = block2->start;

        newLine->prev->size = caret->sym;
        block->size = blockPos;
        block->next = NULL;

        caret->line = newLine;

        if (newLine->prev != NULL) {
            newLine->number = newLine->prev->number;
            newLine->prev->next = newLine;
        } else {
            data->firstLine = newLine;
            newLine->number = 0;
        }

        data->linesSize++;

        while (newLine != NULL) {
            newLine->number++;
            newLine = newLine->next;
        }
        //++caret->sym;
    }
}


void DeleteSym(data_t * data, caret_t * caret, pos_t * pos, int mode) {
    int blockPos = 0;
    int totalSize = 0;
    int prevLineSize = 0;

    block_t * block2;

    block_t * block = caret->line->first;
    if (mode == DEFAULT) {

        // ¬ычисл€ем блок и его позицию, куда указывает курсор
        while (caret->sym + 0 > totalSize + block->size) {
            totalSize += block->size;
            block = block->next;
        }
        blockPos = caret->sym - totalSize;

        // если символ первый в блоке
        if (blockPos == 0) {
            if (caret->line->prev == NULL) {
                return;
            }

            // найдем последний блок в предыдущей строке
            block_t * prevLast = caret->line->prev->first;
            while (prevLast->next != NULL) {
                prevLast = prevLast->next;
            }

            //соединим блоки
            prevLast->next = block;
            block->prev = prevLast;

            // ”величим размер предыдущей строки, но сохраним исходный
            prevLineSize = caret->line->prev->size;
            caret->line->prev->size += caret->line->size;

            //теперь удалим строку и склеим ее соседей
            caret->line->prev->next = caret->line->next;
            if (caret->line->next != NULL) {
                caret->line->next->prev = caret->line->prev;
            }

            line_t * prevLine = caret->line->prev;
            free(caret->line);

            // »зменим положение каретки
            caret->line = prevLine;
            caret->sym = prevLineSize;

            // Ќаведем пор€док в струтуре данных, изменив количество строк и их номера
            prevLine = prevLine->next;
            while (prevLine != NULL) {
                prevLine->number--;
                prevLine = prevLine->next;
            }
            data->linesSize--;

            // ”даление первого пустого блока, если мы переместились в пустую строку
            if (caret->line->first->size == 0) {
                free(caret->line->first);
                caret->line->first = block;
            } else {
                // ”даление блока, если мы были в пустой строке
                if (block->next == NULL && block->size == 0) {
                    block->prev->next = NULL;
                    free(block);
                }
            }

            return;
        }

        if (blockPos == block->size) {

            block->size--;
            caret->line->size--;
            caret->sym--;

            if (block->size == 0) {
                if (caret->line->size == 0) {
                    return;
                }
                if (caret->line->first == block) {
                    caret->line->first = block->next;
                    free(block);
                    return;
                }
                if (block->prev != NULL) {
                    block->prev->next = block->next;
                }
                if (block->next != NULL) {
                    block->next->prev = block->prev;
                }
                free(block);
            }

            //++caret->sym;
            return;
        }

        // если символ не первый и не последний
        block2 = malloc(sizeof(block_t));

        block2->next = block->next;
        block2->prev = block;

        block->next = block2;
        block2->start = block->start + blockPos;
        block2->size = block->size - blockPos;

        block->size = blockPos - 1;
        if (block->size == 0) {
            block2->prev = block->prev;
            if (block->prev == NULL) {
                caret->line->first = block2;
            } else {
                block->prev->next = block2;
            }
            free(block);
        }

        caret->sym--;
        caret->line->size--;
        //++caret->sym;
    }
}


void RecountAdd(data_t * data, caret_t * caret, pos_t * pos, int mode) {
    if (mode == DEFAULT) {
        CaretRight(caret, data,pos);
        if (caret->line->size > data->longestSize) {
            data->longestSize = caret->line->size;
        }
    }
}

