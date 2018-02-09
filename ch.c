/* ch.c.
auteurs:
Amélie Lacombe Robillard (20016735)
Christophe Apollon-Roy (920403)
date:
10/02/2018
problèmes connus:

  */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

int delimCounter(char *string, const char *delim);

void ezParser(char *srcString, char *dstArray[], 
    size_t dstSize, const char *delim);

int bigBoyParser(char *line);

int executeCommand(char *args[], int argsSize);

char *resolveVar(char *token, char *pt);

int main (void){
    fprintf (stdout, "%% ");

    /* ¡REMPLIR-ICI! : Lire les commandes de l'utilisateur et les exécuter. */

    const char equal[2] = "=";
    
    int exitValue = 1;
        
    while(exitValue) {
        //shell loops until "exit" is entered
        char *line = NULL;
        size_t length = 0;
        ssize_t read;
        
        if (isatty(STDIN_FILENO)){fprintf (stdout, "\nour-shell> ");}

        // Read input from keyboard
        read = getline(&line, &length, stdin);
        
        //eof, generated by CRTL+D in Unix and CRTL+Z in Windows makes getline 
        //and getchar return -1, thus we check for it to exit the shell
        if (read == -1){ exitValue = 0;}
        
        // Remove end of line character
        size_t len = strlen(line);
        if(len > 0 && line[len-1] == '\n') {
          line[--len] = '\0';
        }

        
        //To avoid errors if the users types ENTER without entering a command
        if(strcmp(line, "\0") == 0){
        
        }else if(strcmp(line, "exit") == 0){
            exitValue = 0;
        }else if(strchr(line, '=') && strchr(line, ' ') == NULL) {
            // 2. Gestion des variables
            char *var = strtok(line, equal);
            char *val = strtok(NULL, equal);
            setenv(var, val, 1);
            
        }else{
            //Parses complex commands including for, && and ||
            bigBoyParser(line);    
        }                    
               
        fflush(stdout);
    }
    fprintf (stdout, "Bye!\n");
    exit (0);
}

int delimCounter(char *string, const char *delim){
    //Counts number of delimiters in a string. Used for parsing all the things
    size_t len  = strlen(string);
    int i, separators;
    for (i=0, separators=0; i < len; i++) {
        separators += (string[i] == *delim);
    }
    return separators;
}

void ezParser(char *srcString, char *dstArray[], 
    size_t dstSize, const char *delim){
    //tokenizes a string to a destination array according to a delimiter
    int j=0;
    char *token = strtok(srcString, delim);
    
    while(token) {
        // Check if token contains a variable
	char* pt =  strchr(token, '$');
	while(pt) {
		token = resolveVar(token, pt);
		pt = strchr(token, '$');
	}
	dstArray[j] = token;
        token = strtok(NULL, delim);
        j++;
    }
    dstArray[j] = NULL;

}

int bigBoyParser(char *line){
    //Parses all the things
    
    const char space[2] = " ";
    const char semiColon[2] = ";";
    int errorValue = 0;
    
    if(strstr(line, "for ") == line && 
        strcmp(line+(strlen(line)-6), "; done") == 0 && 
        strstr(line, "; do") == strstr(line, ";")){

        //Parsing for loops        
            
        //initialization and body of for in strings parsed differently
        char *init = strtok(line, semiColon);
        char *body = strtok(NULL, "");                
        if (body[strlen(body)-7] == ' '){
            
            body[strlen(body)-7] = '\0';
        }else{body[strlen(body)-6] = '\0';}
        
        //Parse arguments of for loop initialization
        int spaces = delimCounter(init, space);
        char *initArgs[spaces+1];
        ezParser(init, initArgs, sizeof initArgs, space);
        int initLen = sizeof(initArgs)/sizeof(initArgs[0]);
                                       
        //in "for i in A B C ..." the first value is 4th in the array
        for(int i = 3; i < initLen-1; i++){
            
            setenv(initArgs[1], initArgs[i], 1);
            char copy[strlen(body)];
            strcpy(copy, body);
    
            errorValue = bigBoyParser(copy+4);
        }       
        
    }else if(strstr(line, "&&") || strstr(line, "||")){
        
        //searching first occurences of && and ||
        int andOperator = (strstr(line, "&&") != NULL);
        int orOperator = (strstr(line, "||") != NULL);
        
        char *command;
        char *rest;
        
        if(andOperator == 1 && orOperator == 1){
            
            if(strstr(line, "&&") < strstr(line, "||")){
                orOperator = 0;
            }else{
                andOperator = 0;
            }
        }
        
        //separating the commands before and after the operator
        if(andOperator){
            
            command = strtok(line, "&");
            rest = strtok(NULL, "&");
            //replacing the remaining space at the end of command with \0
            command[strlen(command)-1] = '\0';
        }else{
            
            command = strtok(line, "|");
            rest = strtok(NULL, "|");
            //replacing the remaining space at the end of command with \0
            command[strlen(command)-1] = '\0';
        }
              
        int waitError = bigBoyParser(command);
        
        // Logique du && et ||            
        if(andOperator && waitError == 0) {
            
            bigBoyParser(rest+1);	        
        }else if (orOperator && waitError != 0){
            
            bigBoyParser(rest+1);
        }
        
    }else{
        
        //catch-all case for "simple" commands
        int spaces = delimCounter(line, space);
            
        //Parse arguments. There's +1 word than spaces, +1 for NULL at the end
        char *args[spaces+2]; 
        ezParser(line, args, spaces+2, space);
                          
        errorValue = executeCommand(args, spaces+2);
    }
    
    return errorValue;
}

int executeCommand(char *args[], int argsSize){
    
    int waitError;   
    //1. Crée un processus qui exécute la commande
    pid_t  pid;
    pid = fork();
    errno = 0;
    
    if (pid < 0) {
        fprintf (stderr, "Fork failed");
    }
    if (pid == 0) {
        
        //Child process
        if(strcmp(args[0], "cd") == 0){
            //so that the || and && operators can be used on the cd command
            if(args[1] == NULL){
                chdir(getenv("HOME"));
            }else{chdir(args[1]);}
            
        }else if(strcmp(args[0], "exit") != 0){execvp(args[0], args);}
	    
	    perror(args[0]);
	    exit(1);
    }
    else {
        //Parent process
                
        wait(&waitError);
        
        //because cd needs to affect the parent, ie: the shell
        if(strcmp(args[0], "cd") == 0){
            
            if(args[1] == NULL){
                chdir(getenv("HOME"));
            }else{chdir(args[1]);}
            
            waitError = errno;
        }	    
    }
    
    return waitError;
}

char *resolveVar(char *token, char *pt) {
	// Resolve one variable occurence in a string at pointer position pt
	size_t tklen = strlen(token);
	char var[tklen];
	int k=0;
	pt[0] = '\0';
	pt++;
	// Read variable until invalid character or end of string
	while(pt[0] != '\0' && (isalnum(pt[0]) || pt[0] == '_')) {
		var[k]=pt[0];
		pt++;
		k++;
	}
	var[k] = '\0';
	char* val = getenv(var);

	// Rebuild new string with variable value
	size_t newlen = tklen - k + strlen(val);
	char *newToken = malloc(newlen+1);
	strcpy(newToken, token);
	strcat(newToken, val);
	strcat(newToken, pt);

	return newToken;
}
