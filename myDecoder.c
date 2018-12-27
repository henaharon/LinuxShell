#include "myDecoder.h"

char **commandHistory, **environ;
int myPidsSize, myPids[COMMAX];
int mappingPidsHistory[COMMAX];

int HenAtoi(char *s){
	if(s == NULL){
		fprintf(stderr, "betterAtoi() empty string\n");
		return 0;
	}
	int n = 0;
	while(*s != '\0' && *s != '\n'){
		if((!(*s <= '9' && *s>= '0'))){
			return 0;
		}
		n = (*s - '0') + (n * 10);
		s++;
	}
	return n;
}

int initArrays(){
    //allocate commandHistory array
    commandHistory = (char**)malloc(sizeof(char*) * COMMAX);
    if (commandHistory == NULL){
        printf("error allocating commandHistory aray ");
        return 0;
    }
    myPidsSize=0;
	//allocate environ arrays
	environ = (char**)malloc(sizeof(char*) * COMMAX);
	 if (environ == NULL){
        printf("error allocating env aray ");
        return 0;
    }
    return 1;
}


// read line
char *myShell_read_line(void)
{
    char *line = NULL;
    ssize_t bufsize = 0; // have getline allocate a buffer for us
    getline(&line, &bufsize, stdin);
    return line;
}

// split line
char **myShell_split_line(char *line)
{
    int position = 0, isEqueal = 0 , isExcl = 0;
	char **tokens = (char**)malloc(LINEBUFFER * sizeof(char *));
	char *token, *lineCpy;
	lineCpy = (char*)malloc(sizeof(char) * (strlen(line) + 1));
	if(lineCpy == NULL){
		perror("malloc err in splitLine()");
		return NULL;
	}
	strcpy(lineCpy, line);
	if (!tokens)
	{
		fprintf(stderr, "splitLine: malloc error\n");
		return NULL;
	}
	//
	//key=value
	while(*lineCpy != '\0'){
		if(*lineCpy == '=') isEqueal = 1;
		
		if(*lineCpy == '!') isExcl = 1;

		lineCpy++;
	}
	//
	//split line
	token = strtok(line, TOKTOIGNORE);
	while (token != NULL)
	{

		tokens[position] = token;
		position++;

		if (position > LINEBUFFER - 1) {
			printf("too many words in line: %s\n", line);
			break;
		}
		token = strtok(NULL, TOKTOIGNORE);
	}
    //terminate
	tokens[position] = NULL;
	//
	//simple commands
	if (strcmp(tokens[0], "tasks") == 0 || strcmp(tokens[0], "return") == 0 || strcmp(tokens[0], "print_env") == 0 || strcmp(tokens[0], "show_history") == 0) {
        return tokens;
	}
	//
	//key=value
	if (argsCount(tokens) == 2 && isEqueal == 1) {
		//switching positions
        tokens[2] = tokens[1];
		tokens[1] = tokens[0];
		//tokens[0] = '=';
		tokens[0] = (char*)malloc(sizeof(char) * 2);
        if (!tokens[0])
        {
            printf("splitLine: malloc error\n");
            return NULL;
        }
		strcpy(tokens[0], "=");
		//adding NULL to the end
		tokens[3] = NULL;
		return tokens;
	}
	//!
	if(argsCount(tokens) == 1 && isExcl == 1){
		tokens[1] = tokens[0];
		//tokens[0] = "!";
		tokens[0] = (char*)malloc(sizeof(char) * 2);
        if (!tokens[0])
        {
            fprintf(stderr, "splitLine: malloc error\n");
            return NULL;
        }
		strcpy(tokens[0], "!");
		tokens[2] = NULL;
		return tokens;
	}
	//
	int isAmper = 0, isOutput = 0, i , argsTemp;
	//
	//&
    argsTemp = argsCount(tokens);
	if (argsTemp >= 2 && strcmp(tokens[argsTemp - 1], "&") == 0) {
		isAmper = 1;
		//push "&" at the beginning of tokens && remove it from tokens[last]
		for (i = argsTemp - 1; i >= 1; i--) {
			tokens[i] = tokens[i - 1];
		}
		//tokens[0] = myAmper;
		tokens[0] = (char*)malloc(sizeof(char) * 2);
        if (!tokens[0])
        {
            fprintf(stderr, "splitLine: malloc error\n");
            return NULL;
        }
		strcpy(tokens[0], "&");
		return tokens;
	}
	int tmp;
	//
	//output ">"
	if (argsTemp >= 3 && strcmp(tokens[argsTemp - 2], ">") == 0) {
		isOutput = 1;
		//push ">" at the beginning of tokens && remove ">" from tokens[second last]
		for (i = argsTemp - 2; i >= 1; i--) {
			tokens[i] = tokens[i - 1];
		}
		//tokens[0] = '>';
		tokens[0] = (char*)malloc(sizeof(char) * 2);
        if (!tokens[0])
        {
            printf("splitLine: malloc error\n");
            return NULL;
        }
		strcpy(tokens[0], ">");
		return tokens;
	}
	//
	//l
	if (isAmper == 0 && isOutput == 0) {
		//push "l" at the beginning of tokens
		for (i = argsTemp; i >= 1; i--) {
			tokens[i] = tokens[i - 1];
		}
		tokens[0] = (char*)malloc(sizeof(char) * 2);
        if (!tokens[0])
        {
            printf("splitLine: malloc error\n");
            return NULL;
        }
		strcpy(tokens[0], "l");
		//adding NULL to the end
		tokens[++argsTemp] = NULL;
		return tokens;
	}
    printf("end of splitLine! tokens:\n");
}


//counts how many args
int argsCount(char **args){
    if(args == NULL)    return 0;
    int c = 0;
    while(*args++ != NULL) c++;
    return c;
}
