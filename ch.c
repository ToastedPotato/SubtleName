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

int main (void)
{
  fprintf (stdout, "%% ");

  /* ¡REMPLIR-ICI! : Lire les commandes de l'utilisateur et les exécuter. */
  
  //TODO: Lire la commande et parser les arguments
  
  //1. Exécuter la commande - echo / cat / ls / man / tail
  pid_t  pid;
  pid = fork();
  
  if (pid == 0) {
	//ChildProcess();
	execlp("ls", "ls", NULL);
	fprintf (stdout, "Child finished ");
  }
  else {
     //ParentProcess();
     wait(NULL);
	 fprintf (stdout, "Parent finished");
  }
  fprintf (stdout, "End of main");
  
  //TODO: 2. Gestion des variables  
  //TODO: 3. For
  //TODO: 4. && et ||

  fprintf (stdout, "Bye!\n");
  exit (0);
}
