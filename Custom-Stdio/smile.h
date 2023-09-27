#ifndef smile_h_
#define smile_h_

#include "mile.h"

int mgetc(MILE *m);
int mungetc(int c, MILE *m);
int mputc(int c, MILE *m);
int mwrite_int(int i, MILE *m);
int mread_int(MILE *m);

#endif
