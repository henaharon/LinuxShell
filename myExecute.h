#ifndef MYEXECUTE 
#define MYEXECUTE

#include "myDecoder.h"

extern const char *commsCodes[];
extern const int (*commsFuncs[]) (char **);

int myShell_execute(char **args);
int launch(char **args);
int commsNum();
int launchAndWait(char **args);
int launchAndContinue(char **args);
int tasks(char **args);
int returnPid(char **args);
int redirectOut(char **args);
int setNewEnv(char **args);
int printEnv(char **args);
int showHistory(char **args);
int exclMark(char **args);


#endif