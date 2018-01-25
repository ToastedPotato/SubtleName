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
  // TODO LOOP  
  
  const char s[2] = " "; 
  char *line = NULL;
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

  // Parse command and args
  cmd = strtok(line, s);
  arg = strtok(NULL, s);
  // TODO : parse more than one arg

  //1. Exécuter la commande - echo / cat / ls / man / tail
  pid_t  pid;
  pid = fork();
  
  if (pid == 0) {
    //Child process
    execlp(cmd, cmd, arg, NULL);
    //execlp(cmd, cmd, NULL);
    fprintf (stdout, "Child finished ");
  }
  else {
     //Parent process
     wait(NULL);
     fprintf (stdout, "Parent finished");
  }
  
  //TODO: 2. Gestion des variables  
  //TODO: 3. For
  //TODO: 4. && et ||

  free(line);

  fprintf (stdout, "Bye!\n");
  exit (0);
}
