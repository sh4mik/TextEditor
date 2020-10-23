#ifndef WRITE_H_INCLUDED
#define WRITE_H_INCLUDED

int MyTextOut(HDC hdc, data_t * data, pos_t * pos, rsize_t * rSize, TEXTMETRIC * tm); // + pos.abs detection

int MyPrint(HDC hdc, data_t * data, pos_t * pos, rsize_t * rSize, TEXTMETRIC * tm);

#endif
