#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>

#include <dirent.h>

// The following allows us to add debugging statements that are enabled
// Or disabled at compile time
#ifdef DEBUG
#define PDEBUG 1
#else
#define PDEBUG 0
#endif

#define DPRINT(fmt, ...) do { if (PDEBUG) fprintf(stderr, "%s:%d:%s(): "fmt,\
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while (0)


void print_usage() {
    fprintf(stderr, "Usage: myls [-aFs] path\n");
}

struct dentry {
	char *dname; 			// File name
	int dlen; 				// File name's length
	char char_code;			// File character code
	int file_size;			// File size
	char *owner;			// File owner
	char *group;			// File group
	char lastModified[28];	// File last modified date
};

struct dentry *readDirectory(const char *path, int *num, int aflag, int Fflag) {
	DIR *dp;
	struct dirent *de;
	int dcount = 0;
	struct dentry *data = NULL;
	int rcount = 0;

	char ch = ' ';

	struct stat s;

	struct passwd *pw = NULL;
	struct group *gr = NULL;

	dp = opendir(path);
	if (dp == NULL) {
		DPRINT("%s is not a valid path\n",path);
		return NULL;
	}
	
	while ((de = readdir(dp)) != NULL) dcount++;
	rewinddir(dp);

	data = (struct dentry *)malloc(dcount*sizeof(struct dentry));

	while ((de = readdir(dp)) != NULL) {
		DPRINT("found: %s entry\n",de->d_name);

		if(aflag == 0 && de->d_name[0] == '.')
			continue;

		data[rcount].dname = strdup(de->d_name);
		data[rcount].dlen = de->d_namlen;

        // Get full file path name
        char *fullname = malloc(strlen(path) + 1 + strlen(de->d_name));
        sprintf(fullname, "%s/%s", path, de->d_name);
        //printf("%s\n", fullname);
        int returnSZ = stat(fullname, &s);

        // Get file character code
        if(Fflag == 1) {
			if(S_ISDIR(s.st_mode))
				ch = '/';
			else if(s.st_mode & S_IXUSR)
				ch = '*';
			else if(S_ISLNK(s.st_mode))
				ch = '@';
			else if(S_ISFIFO(s.st_mode))
				ch = '|';
			else
				ch = ' ';
		}
		data[rcount].char_code = ch;



        // Get file size
        data[rcount].file_size = s.st_size;

        // Get owner and group info
        pw = getpwuid(s.st_uid);
        gr = getgrgid(s.st_gid);
        data[rcount].owner = pw->pw_name;
        data[rcount].group = gr->gr_name;

        // Get data and time for last modified date
        strftime(data[rcount].lastModified, sizeof(data[rcount].lastModified), "%b %d %H:%M", localtime(&s.st_mtime));
      	
		rcount++;
	}
	closedir(dp);

	*num = rcount;
	return data;

}

int name_cmp(const void *p1, const void *p2) {
	struct dentry *left = (struct dentry *) p1;
	struct dentry *right = (struct dentry *) p2;

	return strcmp(left->dname, right->dname);
}

int main(int argc, char *argv[]) {
    char *path = NULL;

    // Flags for options
    // a = show hidden files; F = show types of files; s = sort by name
    int aflag = 0;
    int Fflag = 0;
    int sflag = 0;

    // If there is not enough arguments
    if(argc < 2) {
    	printf("Error! Not enough arguments given!\n");
    	return -1;
    }

    // argc = 3 => flags; else => no flags
    if(argc == 3) {
    	if(strchr(argv[1], 'a')) {
    		//printf("aflag is set\n");
    		aflag = 1;
    		path = argv[2];
    	} 

    	if(strchr(argv[1], 'F')) {
    		//printf("Fflag is set\n");
    		Fflag = 1;
    		path = argv[2];
    	}

    	if(strchr(argv[1], 's')) {
    		//printf("sflag is set\n");
    		sflag = 1;
    		path = argv[2];
    	}
    } else {
    	path = argv[1];
    }

    // Actual file size
    int size;
	struct dentry *data = NULL;
	int i;

	//data = readDirectory("/home/students/njohn/fall2023/ece322/hw1", &size, aflag, Fflag);
	data = readDirectory(path, &size, aflag, Fflag);

	if(sflag == 1) {
		qsort(data, size, sizeof(struct dentry), name_cmp);
	}

	if(Fflag == 0) {
		for(i = 0; i < size; i++) {
			printf("%-20s %8lld [%s, %s] %s\n", 
			   	   data[i].dname, data[i].file_size, 
			   	   data[i].owner, data[i].group, data[i].lastModified);
		}
	} else {
		for(i = 0; i < size; i++) {
			printf("%-20s [%c] %8lld [%s, %s] %s\n", 
			   	   data[i].dname, data[i].char_code, data[i].file_size, 
			   	   data[i].owner, data[i].group, data[i].lastModified);
		}
	}

    // Check if a path is provided
    if (path == NULL) {
        print_usage();
        return -1;
    }

    // Open the directory
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return -1;
    }

    closedir(dir);

    return 0;
}