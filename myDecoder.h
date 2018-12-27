#ifndef MYDECODER
#define MYDECODER


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


#define LINEBUFFER 64
#define COMMAX 128
#define TOKTOIGNORE "=! \t\r\n\a"


extern char** environ; // Environment variables
extern char **commandHistory; // Commands array 
extern int myPidsSize; // Pids array size
extern int myPids[COMMAX]; // Pids array
extern int mappingPidsHistory[COMMAX]; // Mapping Commands and Pids array


int HenAtoi(char *s);
int initArrays();
char *myShell_read_line(void);
char **myShell_split_line(char *line);
int argsCount(char **args);





#endif
