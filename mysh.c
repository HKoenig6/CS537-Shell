#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static int BUFFER_SIZE = 512; //generic buffer size

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
  char prompt[7];
  strcpy(prompt, "mysh> ");
  write(STDOUT_FILENO, prompt, strlen(prompt));
  while(fgets(buffer, BUFFER_SIZE, fd == NULL ? stdin : fd)) {
    write(STDOUT_FILENO, prompt, strlen(prompt));
  }
}
