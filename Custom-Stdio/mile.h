#ifndef mile_h_
#define mile_h_

typedef struct MILE {
	int fd;
	char mode;
	void *buffer;
	int size;
	int readposition;
	int writeposition;
	int currentposition;
}MILE;

int mread(void *b, int len, MILE *m);
int mwrite(void *b, int len, MILE *m);
int mclose(MILE *m);
MILE *mopen(char *name, char *mode);

#endif
