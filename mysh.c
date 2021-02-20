#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static int BUFFER_SIZE = 512; //generic buffer size
static const char delim[3] = " \t\n";

int main(int argc, char *argv[]) {
  FILE *fd = NULL;	
  if (argc > 1) {
    fd = fopen(argv[1], "r");
    if (fd == NULL) {
      write(STDOUT_FILENO, "Error: cannot open file\n", BUFFER_SIZE);
      exit(1);
    }
  }
  char buffer[BUFFER_SIZE];
  char prompt[7] = "mysh> ";
  char *cmd[100]; //initial parse
  //int pid = 0;
  int i = 0; //index of cmd arg
  write(STDOUT_FILENO, prompt, strlen(prompt));
  while(fgets(buffer, BUFFER_SIZE, fd == NULL ? stdin : fd)) {
    cmd[i] = strtok(buffer, delim);
    while (cmd[i] != NULL) {
      i++;
      cmd[i] = strtok(NULL, delim);
    }
    char *args[i + 1];
    for (int j = 0; j < i + 1; j++) {
      args[j] = cmd[j]; //eliminates null elements at end of cmd with args
      write(STDOUT_FILENO, args[j] != NULL ? args[j] : "NULL", args[j] == NULL ? 5 : strlen(args[j]));
      write(STDOUT_FILENO, "\n", 2);
    }

    /*pid = fork();
    if (!pid) { //child
      char *args = strtok(buffer, " ");
      exit(1); //exec failed
    } else { //parent
      wait();
    }*/
    i = 0;
    write(STDOUT_FILENO, prompt, strlen(prompt));
  }
}
