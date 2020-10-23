#ifndef CARET_H_INCLUDED
#define CARET_H_INCLUDED

#include "struct.h"

#define CARET_SIZE_COEFF 1

typedef struct caret {
    line_t * line;
    int sym;
    int w;  // caret width
    int h;  // caret height
} caret_t;

void NewCaret(data_t * data, caret_t * caret, TEXTMETRIC tm);

void CaretUp(caret_t * caret, data_t * data, pos_t * pos);
void CaretDown(caret_t * caret, data_t * data, pos_t * pos);
void CaretLeft(caret_t * caret, data_t * data, pos_t * pos);
void CaretRight(caret_t * caret, data_t * data, pos_t * pos);

void CaretDraw(caret_t * caret, data_t * data, pos_t * pos, TEXTMETRIC * tm, rsize_t * rSize);

#endif
