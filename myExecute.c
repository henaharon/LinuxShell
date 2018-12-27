//
// Students:
//Hen Aharon 307887596
//Chen Yafe 311132427
#include "myExecute.h"

const char *commsCodes[]  = {
    "l",            //args[0] = l, args[1] = path, args[..] = arguments
    "&",            //args[0] = '&', args[1] = path
    "tasks",        //args[0] = "tasks"
    "return",       //args[0] = "return" args[1] = pid
    ">",            //args[0] = '>', args[1] = output, args[2] = path, args[..] = arguments
    "=",            //args[0] = '=', args[1] = KEY, args[2] = VALUE
    "print_env",     //args[0] = "print_env",
    "show_history", //args[0] = "show_history"
    "!"             //args[0] = '!', args[1] = path(from commandHistory array)
    };
//commands matching function (order is important!)
const int (*commsFuncs[])(char **) = {
    &launchAndWait,
    &launchAndContinue,
    &tasks,
    &returnPid,
    &redirectOut,
    &setNewEnv,
    &printEnv,
    &showHistory,
    &exclMark
    };
//

int myShell_execute(char **args){ // execute al command, CMD commands continue to launch
    int i, r = 1, numOfComm;

    if (args[0] == NULL)
    {
        // An empty command was entered.
        return 0;
    }
    //
    //connecting to the relevant command
    numOfComm = commsNum();
    for (i = 0; i < numOfComm; i++)
    {
        // error in myShell_split_line function , No command
        if(args== NULL){
            return -1; 
        }
        //all commands
        if (strcmp(args[0], commsCodes[i]) == 0){
            // activate the correct command 
            r = (*commsFuncs[i])(args);
            //CMD commands
            if(r == 1 && (strcmp(args[0], "l") == 0 || strcmp(args[0], "&") == 0 || strcmp(args[0], ">") == 0 || strcmp(args[0], "!") == 0))
                r = launch(args);
            break;
        }
    }

}


int launch(char **args){

    int pid, wpid;
    int status, comm = -1, i, lenT,outFD,s;
    char **argv, *newOut = NULL;
    int numOfArgs = argsCount(args);
    //argv for exec()
    argv = (char**)malloc(LINEBUFFER * sizeof(char *));
    //create argv for lanchAndWait
    if(strcmp(args[0], "l") == 0){
        comm = 0;
        for(i = 1; i < numOfArgs; i++){
            lenT = strlen(args[i]);
            argv[i-1] = (char*)malloc(sizeof(char)*(lenT+1));
            if(argv[i-1] == NULL){
                perror("malloc err in launch()");
                return 0;
            }
            strcpy(argv[i-1], args[i]);
        }
        argv[++i] = NULL;
    }
    //create argv for launchAndContinue
    if(strcmp(args[0], "&") == 0){
        comm = 1;
        for(i = 1; i < numOfArgs; i++){
            lenT = strlen(args[i]);
            argv[i-1] = (char*)malloc(sizeof(char)*(lenT+1));
            if(argv[i-1] == NULL){
                perror("malloc err in launch()");
                return 0;
            }
            strcpy(argv[i-1], args[i]);
        }
        argv[++i] = NULL;
    }
    //create argv for runAppRedirectOutput&Wait
    if(strcmp(args[0], ">") == 0){
        comm = 2;
        for(i = 1; i < numOfArgs - 1; i++){
            lenT = strlen(args[i]);
            argv[i-1] = (char*)malloc(sizeof(char)*(lenT+1));
            if(argv[i-1] == NULL){
                perror("malloc err in launch()");
                return 0;
            }
            strcpy(argv[i-1], args[i]);
        }
        argv[++i] = NULL;
        lenT = strlen(args[numOfArgs - 1]);
        newOut = (char*)malloc(sizeof(char) * (lenT + 1));
        if(newOut == NULL){
            perror("malloc err in launch()");
            return 0;
        }
        strcpy(newOut, args[numOfArgs - 1]);
    }

    //OUTPUT redirection
    if(comm == 2){
        s = argsCount(args);
        outFD = open(args[s-1], O_WRONLY | O_CREAT | O_TRUNC,S_IRWXU);
    }

    //fork
    pid = fork();
    if (pid == 0)
    {
          //change output stream
        if(comm == 2){
            if(close(STDOUT_FILENO) < 0){
                perror("close didn't work in OUTPUT redirect");
                return 0;
            }
            dup(outFD);
        }
        // Child process
        int a;
        if (execv(args[1], argv) == -1)
        {
            if(errno==ENOENT)// Using errno for cheking wrong file/directory error
            {
                perror("Error:execv in launch()");
                return 1;
            }
            perror("Error:execv in launch()");
            return 0;
        }
        return 1;
    }
    else if (pid < 0)
    {
        // Error forking
        perror("Error :fork in launch()");
        return 0;
    }
    else
    {
        // Parent process
        //wait if needed
        if(comm == 0 || comm == 2){
            do
            {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
        //save pif if needed

        if(comm == 1){

            // save pid
            myPids[myPidsSize] = pid;
            // mapping p
            mappingPidsHistory[myPidsSize] = argsCount(commandHistory) - 1;  
            myPidsSize++;  
        }
    }
    int sTmp = argsCount(argv);
    for(i = 0; i < sTmp; i++){
        if(argv[i] != NULL) free(argv[i]);
    }
    if(argv != NULL) free(argv);
    if(newOut != NULL) free(newOut);
    return 1;


}


int commsNum()
{
    return sizeof(commsCodes) / sizeof(char *);
}
 /// ALL COMMANDS IMPLENTATION
 /// COMMANDS WITH return 1 are handled in launch()

int launchAndWait(char **args){
    return 1;
}

int launchAndContinue(char **args){
    return 1;
}

// Printing all tasks in background
int tasks(char **args){

     if(myPidsSize == 0){
        printf("No proccess in background\n");
        return 1;
    }
    char **argsTmp , hisCopy[2048];
    int currPid = -1, i;

    for(i = 0; i < myPidsSize; i++){
        currPid = myPids[i];
        strcpy(hisCopy,commandHistory[mappingPidsHistory[i]]);
        argsTmp = myShell_split_line(hisCopy);
        printf("> %s:%d\n", argsTmp[1], currPid);
        currPid = -1;
    }
    return 1;
    
}

// Waiting for proccess to be killed
int returnPid(char **args){
     if(args[1] == NULL){
        fprintf(stderr, "no pid for command return\n");
        return 0;
    }
    char *tmpS;
    int len = strlen(args[1]);
    tmpS = (char*) malloc(sizeof(char) * (len+1));
    //check
    strcpy(tmpS, args[1]);
    int currPid = HenAtoi(tmpS);
    if(currPid == 0){
        printf(" Error:no valid pid for args[1] = %s\n", args[1]);
        return 0;
    }
    int i, flag = 0, pidI, status, wpid;
    for(i = 0; i < myPidsSize; i++){
        if(myPids[i] == currPid){
            do
            {
                wpid = waitpid(currPid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            flag = 1;
            pidI = i;
            break;
        }
    }
    if(flag == 0){
        printf("Error:requested pid = %d wasn't found\n", currPid);
        return 0;
    }
    if(flag == 1){
        //remove myPids[i] & mappingPidsHistory[i]
        for(i = pidI; i < myPidsSize - 1; i++){
            myPids[i] = myPids[i+1];
            mappingPidsHistory[i] = mappingPidsHistory[i+1];
        }
        myPidsSize--;
        //print success message
        printf("pid = %d removed!\n", currPid);
        return 1;
    }
}

int redirectOut(char **args){
    return 1;
    
}
int setNewEnv(char **args){
     if(args == NULL || argsCount(args) != 3){
        fprintf(stderr, "not enough args in setNewEnv\n");
        return 0;
    }
    char *key, *val;
    int keyLen, valLen, envsSize = 0, envReturn;
    keyLen = strlen(args[1]);
    valLen = strlen(args[2]);
    key = (char*)malloc(sizeof(char)*(keyLen+1));
    if(key == NULL){
        perror("malloc error in setNewEnv()");
        return 0;
    }
    strcpy(key, args[1]);
    val = (char*)malloc(sizeof(char)*(valLen+1));
    if(val == NULL){
        perror("malloc error in setNewEnv()");
        return 0;
    }
    strcpy(val, args[2]);
    //add to environ
    envsSize = argsCount(environ);
    environ[envsSize] = (char*)malloc(sizeof(char)*(keyLen + 1));
    if(val == NULL){
        perror("malloc error in setNewEnv()");
        return 0;
    }
    strcpy(environ[envsSize], key);
    //terminate environ
    environ[envsSize+1] = NULL;
    //set
    envReturn = setenv(key, val, 1);
    if(envReturn == -1){
        perror("setenv() err");
        return 0;
    }
    printf("Success: %s was added with value %s\n", key, val);
    return 1;
}
int printEnv(char **args){
    int i, envsSize = 0;
    char temp[128];
    envsSize = argsCount(environ);
    if(envsSize == 0){
        printf("Success: no enviorment variables set\n");
        return 1;
    }
    for(i = 0; i < envsSize; i++){
        strcpy(temp,environ[i]);
        printf("KEY: %s, VALUE: %s \n", environ[i],getenv(temp));
    }
    printf("\n");
    return 1;
}
int showHistory(char **args){
    if(commandHistory == NULL){
        fprintf(stderr, "no valid history pointer\n");
        return 0;
    }
    int size, i;
    size = argsCount(commandHistory);
    if(size == 0){
        printf("Success: no history\n");
        return 1;
    }
    for(i = 0; i < size; i++){
        printf("> %d) %s", i, commandHistory[i]);
    }
    return 1;
}


int exclMark(char **args){
    return 1;
}


