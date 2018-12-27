//
// Students:
//Hen Aharon 307887596
//Chen Yafe 311132427
#include "myExecute.h"



int main(int argc,char **argv)
{
    char *line, **args, strTmp[2048];
    int historySize, initFlag, execute=1 ,sTmp,histNum;
    


    if (initArrays() == 0)
    {
        printf("  Arrays:allocation error !\n");
        return 1;
    }



    do {
        printf("> ");
        //get line
        line = myShell_read_line();
        if(line == NULL){
            fprintf(stderr, "no line input!\n");
            continue;
        }
        //split the line
        strcpy(strTmp, line);
        args = myShell_split_line(strTmp);
        //
         // empty line / error 
        if(args == NULL){
            fprintf(stderr, "couldn't splitLine\n");
            if(line != NULL) free(line);
            continue;
        }
        // Getting Commands array size
        historySize = argsCount(commandHistory);
        // if the commend is !(number) ignore
        if(historySize == 0 && strcmp(args[0],"!") == 0)
        {
            if(line != NULL) free(line);
            continue;
        }
        //save line in history
        commandHistory[historySize] = (char*)malloc(sizeof(char) * (strlen(line)+1));
        if(commandHistory[historySize] == NULL){
            printf("allocate error : this command won't be added to array\n");
            free(line);
            return 1;
        } 
            
        strcpy(commandHistory[historySize], line);// Copying line 
        commandHistory[historySize + 1] = NULL; //Advance history array

        // "!" command check
        while(strcmp("!", args[0]) == 0){
            histNum = -1;
            histNum = HenAtoi(args[1]);
            strcpy(strTmp, commandHistory[histNum]);
            args = myShell_split_line(strTmp);
        }

        // Run command from line
        execute = myShell_execute(args);
        if(execute == -1){
            fprintf(stderr, "Wrong input: %s", line);
        }
        if(execute == 0){
            fprintf(stderr, "myShell_execute() didn't work\n");
        }

        if(line != NULL)    free(line);
        if(args == NULL){
            continue;
        }
        
        if(args != NULL){
            free(args);
        }
    } while (execute);

}
