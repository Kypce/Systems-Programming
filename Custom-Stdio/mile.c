#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "mile.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void resetbuffer(MILE *m) {
	m->readposition = 0;
	m->currentposition = 0;
	m->writeposition = 0;
	memset(m->buffer, 0, m->size);
}

int flush(MILE *m, int len){
	int count = write(m->fd, m->buffer + m->writeposition, len);
	return count;
}

MILE *mopen(char *name, char *mode){
	MILE *mile = (MILE *)malloc(sizeof(MILE));
	mile->size = 100;
	mile->buffer = malloc(mile->size);

	mile->readposition = 0;
	mile->writeposition = 0;
	mile->currentposition = 0;

	if(strcmp(mode, "r") == 0){
		mile->fd = open(name, O_RDONLY);
		mile->mode = 'r';
	}
	if(strcmp(mode, "w") == 0){
		mile->fd = open(name, O_WRONLY | O_TRUNC | O_CREAT, 0600);
		mile->mode = 'w';
	}
	if(strcmp(mode, "a") == 0){
		mile->fd = open(name, O_WRONLY | O_APPEND | O_CREAT, 0600);
		mile->mode = 'a';
	}
	if(mile->fd == -1){
		return NULL;
	} else {
		return mile;
	}
}

int mread(void *b, int len, MILE *m){
	int count = 0;
	if(m->currentposition == 0){
		m->currentposition = read(m->fd, m->buffer, m->size);
		if(m->currentposition == 0){
			return 0;
		}
	}
	if(m->currentposition == m->readposition){
		resetbuffer(m);
		m->currentposition = read(m->fd, m->buffer, m->size);
		if(m->currentposition == 0){
			return 0;
		}
	}
	if(((m->currentposition)-(m->readposition)) > len) {
		memcpy(b, m->buffer + m->readposition, len);
		m->readposition = m->readposition + len;
		count += len;
		if(m->readposition > m->currentposition) {
			m->readposition = 0;
		}
	} else {
		int i = 0;
		while(i<len){
			int diff = m->currentposition - m->readposition;
			memcpy(b, m->buffer + m->readposition, diff);
			count = count + diff;
			m->readposition = m->readposition + diff;
			if(len >= diff) {
				break;
			}
			i = diff + i;
			if(m->readposition == 0) {
				m->currentposition = read(m->fd, m->buffer, m->size);
				if(m->currentposition == 0) {
					return 0;
				}
			}
		}
	}
	return count;
}

int mwrite(void *b, int len, MILE *m) {
	int count = 0;
	int bsize = strlen(b);

	if(m->mode != 'w' && m->mode != 'a')
		return -1;
	while(count < bsize) {
		if(m->size > (bsize - count)) {
			memcpy(m->buffer, b + count, bsize - count);
			count += flush(m, bsize - count);
			resetbuffer(m);
		} else {
			memcpy(m->buffer, b, bsize);
			count += flush(m, m->size);
			resetbuffer(m);
		}
	}
	return count;
}

int mwrite2(void *b, int len, MILE *m) {
	int count = 0;

	if(m->mode != 'w' && m->mode != 'a')
		return -1;
	
	if(m->currentposition == 0){
		memcpy(m->buffer, b, m->size);
		for(int i=0; i<m->size; i++) {
			char *p = m->buffer + i;
			if(*p != 0)
				m->currentposition++;
		}
		if(m->currentposition == 0)
			return 0;
	}
	if(m->currentposition == m->writeposition){
		resetbuffer(m);
		memcpy(m->buffer, b, m->size);
		for(int i=0; i<m->size; i++) {
			char *p = m->buffer + i;
			if(*p != 0)
				m->currentposition++;
		}
		if(m->currentposition == 0)
			return 0;
	}
	if((m->currentposition - m->writeposition) > len) {
		flush(m, len);
		m->writeposition += len;
		count += len;
	} else {
		int i = 0;
		while(i < len) {
			int diff = (m->currentposition - m->writeposition);
			flush(m, diff);
			count += diff;
			m->writeposition = m->writeposition + diff;
			if(len >= diff)
				break;
			i += diff;
			if(m->writeposition == 0) {
				for(int i=0; i<m->size; i++) {
					char *p = m->buffer + i;
					if(*p != 0)
						m->currentposition++;
				}
				if(m->currentposition == 0)
					return 0;
			}
		}
	}
	return count;
}

int mclose(MILE *m){	
	if(m->mode == 'r') {
		resetbuffer(m);
	} else {
		int count = 0;
		for(int i = 0; i < (m->size - m->writeposition); i++) {
			char *pointer = m->buffer + i + m->writeposition;
			if(*pointer != 0)
				count++;
		}
		flush(m, count);
		resetbuffer(m);
	}
	free(m->buffer);
	close(m->fd);
}

