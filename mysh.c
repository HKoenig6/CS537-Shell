#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

static int BUFFER_SIZE = 512; //generic buffer size
static const char delim[4] = "\t\n ";

int main(int argc, char *argv[]) {
  FILE *fd = NULL;	
  if (argc > 1) {
    fd = fopen(argv[1], "r");
    if (fd == NULL) {
      write(STDOUT_FILENO, "Error: cannot open file\n", BUFFER_SIZE);
      _exit(1);
    }
  }
  char buffer[BUFFER_SIZE];
  char prompt[7] = "mysh> ";
  char *cmd[100]; //initial parse
  int cpid = 0;
  int i = 0; //index of cmd arg

  //int redir = 0; //bit flipped when redirection found
  int reindex = 0; //used for when > char is not separated by whitespace
  char *refile = NULL; //file to redirect to

  write(STDOUT_FILENO, prompt, strlen(prompt));
  while(fgets(buffer, BUFFER_SIZE, fd == NULL ? stdin : fd)) {
    //tokenize the buffer
    cmd[i] = strtok(buffer, delim);
    while (cmd[i] != NULL) {
      reindex = strcspn(cmd[i], ">");
      if (reindex < strlen(cmd[i])) { //redirection found
        //redir = 1;
        refile = cmd[i] + reindex + 1;
	char thisToken[reindex + 1];
	for (int k = 0; k < reindex; k++) {
	  thisToken[k] = cmd[i][k];
	}
	thisToken[reindex] = '\0';
	printf("(%s, %s)", thisToken, refile);
      }
      i++;
      cmd[i] = strtok(NULL, delim);
    }
    char *args[i + 1];
    for (int j = 0; j < i + 1; j++) {
      args[j] = cmd[j]; //eliminates null elements at end of cmd with args
    }

    //test for keywords
    if (args[0] == NULL) { //no command line input
      write(STDOUT_FILENO, prompt, strlen(prompt));
      continue;
    } else if (!strcmp(args[0], "exit")) { //exit program
      _exit(0);
    }

    cpid = fork();
    int status;
    if (!cpid) { //child
      execvp(args[0], args);
      _exit(1); //exec failed
    } else { //parent
      waitpid(cpid, &status, WUNTRACED | WCONTINUED);
    }
    i = 0;
    write(STDOUT_FILENO, prompt, strlen(prompt));
  }
}
