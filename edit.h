#ifndef EDIT_H_INCLUDED
#define EDIT_H_INCLUDED

#include "struct.h"
#include "caret.h"
#include "menu.h"

void AddSym(data_t * data, caret_t * caret, pos_t * pos, int mode, char ch);
void AddEnter(data_t * data, caret_t * caret, pos_t * pos, int mode, char ch);

void RecountAdd(data_t * data, caret_t * caret, pos_t * pos, int mode);
#endif // EDIT_H_INCLUDED
