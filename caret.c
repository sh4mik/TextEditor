#include "caret.h"

#include <windows.h>
#include "menu.h"


void NewCaret(data_t * data, caret_t * caret, TEXTMETRIC tm) {
    caret->line = data->firstLine;
    caret->sym = 0;
    caret->w = tm.tmAveCharWidth;
    caret->h = tm.tmHeight;
}

void CaretUp(caret_t * caret, data_t * data, pos_t * pos) {
    if (caret->line->number == 0)
        return;

    int upperStringSize = caret->line->prev->size;

    if (caret->sym > upperStringSize) {
        caret->sym = upperStringSize;
        caret->line = caret->line->prev;
    } else if (caret->sym <= upperStringSize) {
        caret->line = caret->line->prev;
    }
}

void CaretDown(caret_t * caret, data_t * data, pos_t * pos) {
    if (caret->line->number == data->linesSize-1)
        return;

    int nextStringSize = caret->line->next->size;

    if (caret->sym > nextStringSize) {
        caret->sym = nextStringSize;
        caret->line = caret->line->next;
    } else if (caret->sym <= nextStringSize) {
        caret->line = caret->line->next;
    }
}

void CaretLeft(caret_t * caret, data_t * data, pos_t * pos) {
    if (caret->sym == 0) {
        if (caret->line->number == 0) {
            return;
        }
        caret->line = caret->line->prev;
        caret->sym = caret->line->size;
    } else {
        caret->sym--;
    }

}

void CaretRight(caret_t * caret, data_t * data, pos_t * pos) {
    if (caret->line->number == data->linesSize - 1 &&
        caret->sym >= caret->line->size) {
            return;
    }

    if (caret->sym == caret->line->size){
        caret->line = caret->line->next;
        caret->sym = 0;
    } else {
        caret->sym++;
    }

}

void CaretPageUp(caret_t * caret, data_t * data, pos_t * pos, rsize_t * rSize) {
    int i = 0;

    caret->sym = 0;
    for (i = 0; i < rSize->height - 1; ++i) {
        if (caret->line->number == 0)
            return;
        caret->line = caret->line->prev;
    }

}

void CaretPageDown(caret_t * caret, data_t * data, pos_t * pos, rsize_t * rSize) {
    int i = 0;

    caret->sym = 0;
    for (i = 0; i < rSize->height - 1; ++i) {
        if (caret->line == NULL || caret->line->number == data->linesSize-1)
            return;
        caret->line = caret->line->next;
    }
}

void CaretBegin(caret_t * caret, data_t * data) {
    caret->sym = 0;
    caret->line = data->firstLine;
}

void CaretEnd(caret_t * caret, data_t * data) {
    caret->sym = 0;
    while (caret->line != NULL && caret->line->number < data->linesSize-1) {
        caret->line = caret->line->next;
    }
}


void CaretDraw(caret_t * caret, data_t * data, pos_t * pos, TEXTMETRIC * tm, rsize_t * rSize) { // TODO normal

    int width = rSize->width;
    int height = rSize->height;

    // printf("%i %i %i\n", caret->row, pos->v, height);

    if (pos->v > caret->line->number) {
        pos->v = fmax(0, caret->line->number);
    } else if (pos->v + height <= caret->line->number) {
        pos->v = fmin(data->linesSize - height, caret->line->number - height + 1);
    }

    if (pos->h > caret->sym) {
        pos->h = caret->sym;
    } else if (pos->h + width <= caret->sym) {
        pos->h = fmin(caret->line->size - width + 1, caret->sym - width + 1);
    }


}

