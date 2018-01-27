/* ch.c.
auteur:
date:
problèmes connus:

  */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main (void)
{
  fprintf (stdout, "%% ");

  /* ¡REMPLIR-ICI! : Lire les commandes de l'utilisateur et les exécuter. */

  const char s[2] = " ";
  const char eq[2] = "=";

  while(1) { //TODO : end of loop condition
    char *line = NULL;  //ugly variable declarations
    char *cmd = NULL;
    char *arg = NULL;
    size_t length = 0;
    ssize_t read;

    fprintf (stdout, "our-shell> ");

    // Read input from keyboard
    read = getline(&line, &length, stdin);
    //if(read > 0) safety check!!

    // Remove end of line character
    size_t len = strlen(line);
    if(len > 0 && line[len-1] == '\n') {
      line[--len] = '\0';
    }

    // Check if is a variable declaration
    char *pt = strchr(line, '=');
    if(pt) {
      // 2. Gestion des variables
      char *var = strtok(line, eq);
      char *val = strtok(NULL, eq);
      setenv(var, val, 0);
    } else {
      // Count number of spaces - better option would be to precount number
      // of tokens
      int i, spaces;
      for (i=0, spaces=0; i < len; i++) {
        spaces += (line[i] == ' ');
      }

      // Parse arguments
      int j=0;
      char *args[spaces+2]; // +1 word than space, +1 for NULL at the end
      char *token = strtok(line, s);
      while(token) {
        // Check if token is a variable
        if(token[0] == '$') {
          token = getenv(strtok(token, "$"));
        }
        args[j] = token;
        token = strtok(NULL, s);
        j++;
      }
      args[j] = NULL;

      //1. Exécuter la commande - echo / cat / ls / man / tail
      pid_t  pid;
      pid = fork();

      if (pid < 0) {
        fprintf (stderr, "Fork failed");
      } if (pid == 0) {
        //Child process
        execvp(args[0], args);
      }
      else {
         //Parent process
         wait(NULL);
         fprintf (stdout, "Parent finished\n");
      }

      //TODO: 3. For
      //TODO: 4. && et ||

    }
  }
  fprintf (stdout, "Bye!\n");
  exit (0);
}
