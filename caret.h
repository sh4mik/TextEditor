#ifndef CARET_H_INCLUDED
#define CARET_H_INCLUDED

#include "struct.h"

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

void CaretPageUp(caret_t * caret, data_t * data, pos_t * pos, rsize_t * rSize);
void CaretPageDown(caret_t * caret, data_t * data, pos_t * pos, rsize_t * rSize);
void CaretBegin(caret_t * caret, data_t * data);
void CaretEnd(caret_t * caret, data_t * data);

void CaretDraw(caret_t * caret, data_t * data, pos_t * pos, TEXTMETRIC * tm, rsize_t * rSize);

#endif
