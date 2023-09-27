#include "smile.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int mgetc(MILE *m) {
	char c;
	int nr;
	
	if(nr = mread(&c, 1, m) > 0)
		return c;
	else
		return 0;
}

int mungetc(int c, MILE *m) {
	int squirtle = mread(&c, 1, m);
	if(squirtle > 0) {
		m->readposition = m->readposition - 1;
		return c;
	} else
		return 0;
}

int mputc(int c, MILE *m) {
	mwrite(&c, 1, m);
	return 0;
}

int inttoascii(int num) {
	return '0' + num;
}

int mwrite_int(int i, MILE *m) {
	char *c;
	int temp = i;
	int factor = 1;
	int index;
	int amount = 0;

	if(i < 0) {
		i = -i;
		mputc('-', m);
		amount++;
	}
	while(temp) {
		temp = temp / 10;
		factor = factor * 10;
	}
	while(factor > 1) {
		factor = factor/10;
		index = inttoascii(i/factor);
		mputc(index, m);
		amount++;
		i = i % factor;
	}
	return amount;
}

int mread_int(MILE *m) {
	char *c;
	char tmp;
	
	int i = 0;
	while((tmp = mgetc(m)) != 10) {
		if(tmp == 45 || tmp >=48 && tmp <=57)
			strcpy(c + i, &tmp);
		i++;
	}

	i = atoi(c);
	return i;
}
