#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

static int BUFFER_SIZE = 512; //generic buffer size
static const char delim[4] = "\t\n ";

struct aliasNode {
  char *alias;
  char **args;
  int argc;
  struct aliasNode *next;
};

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

  int redir = 0; //bit flipped when redirection found
  int reindex = 0; //used for when > char is not separated by whitespace
  char *refile = NULL; //file to redirect to

  //construct linked list
  struct aliasNode *head = malloc(sizeof(struct aliasNode));

  if (fd == NULL) { write(STDOUT_FILENO, prompt, strlen(prompt));}
  while(fgets(buffer, BUFFER_SIZE, fd == NULL ? stdin : fd)) {
    if (fd != NULL) { write(STDOUT_FILENO, buffer, strlen(buffer));}
    //tokenize the buffer
    cmd[i] = strtok(buffer, delim);
    while (cmd[i] != NULL) {
      reindex = strcspn(cmd[i], ">");
      if (reindex < strlen(cmd[i])) { //redirection found
        redir = 1;
        refile = strdup(cmd[i] + reindex + 1);
	char thisToken[reindex + 1];
	for (int k = 0; k < reindex; k++) {
	  thisToken[k] = cmd[i][k];
	}
	thisToken[reindex] = '\0';
	if (!strcmp(refile, "")) { //space after redir
	  refile = strdup(strtok(NULL, delim));
	}
	if (!strcmp(thisToken, "")) { //space before redir
	  cmd[i] = NULL;
	} else {
	  cmd[i] = thisToken;
	  cmd[i + 1] = NULL;
	  i++;
	}
	break;
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
      i = 0;
      redir = 0;
      if (fd == NULL) { write(STDOUT_FILENO, prompt, strlen(prompt));}
      continue;
    } else if (!strcmp(args[0], "exit")) { //exit program
      _exit(0);
    } else if (!strcmp(args[0], "alias")) { //alias expected
      if (i == 1) { //list all aliases
	struct aliasNode *currNode = head;
	do {
	  if (currNode->alias != NULL && currNode->args != NULL) {
	    write(STDOUT_FILENO, currNode->alias, strlen(currNode->alias));
            write(STDOUT_FILENO, " ", 1);
	    for (int m = 0; m < currNode->argc; m++) {
	      write(STDOUT_FILENO, currNode->args[m], strlen(currNode->args[m]));
              write(STDOUT_FILENO, " ", 1);
	    }
	    write(STDOUT_FILENO, "\n", 1);
	  }
          currNode = currNode->next;	  
	} while (currNode != NULL);
      } else if (i == 2) { //list a specific alias
        struct aliasNode *currNode = head;
        do {
          if (!strcmp(args[1], currNode->alias)) {
	    write(STDOUT_FILENO, currNode->alias, strlen(currNode->alias));
            write(STDOUT_FILENO, " ", 1);
	    for (int m = 0; m < currNode->argc; m++) {
	      write(STDOUT_FILENO, currNode->args[m], strlen(currNode->args[m]));
              write(STDOUT_FILENO, " ", 1);
	    }
	    write(STDOUT_FILENO, "\n", 1);
	    break;
	  }
	  currNode = currNode->next;
	} while (currNode != NULL);
      } else { //construct new alias
	char *aliasArr = malloc(sizeof(args[1]));
	char **argArr = malloc(sizeof(args + 2));
        
	if (head->alias == NULL) { //initialize first list element
          head->alias = aliasArr;
	  strcpy(head->alias, args[1]);
	  head->args = argArr;
	  for (int m = 2; m < i; m++) {
	    char *argElem  = malloc(sizeof(args[m]));
	    head->args[m-2] = argElem;
	    strcpy(head->args[m - 2], args[m]);
	  }
	  head->argc = i - 2;
	} else {
          struct aliasNode *newNode = head;
	  while (newNode->next != NULL) {
	    newNode = newNode->next;
	  }
	  newNode->next = malloc(sizeof(struct aliasNode));
	  newNode->next->alias = aliasArr;
	  strcpy(newNode->next->alias, args[1]);
	  newNode->next->args = argArr;
	  for (int m = 2; m < i; m++) {
	    char *argElem  = malloc(sizeof(args[m]));
	    newNode->next->args[m-2] = argElem;
	    strcpy(newNode->next->args[m - 2], args[m]);
	  }
	  newNode->next->argc = i - 2;
	}
      }
      i = 0;
      redir = 0;
      if (fd == NULL) { write(STDOUT_FILENO, prompt, strlen(prompt));}
      continue;
    } else if (!strcmp(args[0], "unalias")) { //unalias expected
      if (head->alias != NULL && !strcmp(args[1], head->alias)) { //special case: head is removed
        free(head->alias);
	for (int i = 0; i < head->argc + 1; i++) {
	  free(head->args[i]);
	}
	free(head->args);
	struct aliasNode *freedHead = head;
	if (head->next != NULL) {
	  head = head->next; //new head
	} else {
	  head = malloc(sizeof(struct aliasNode));
	}
	free(freedHead);
      } else { //iterate through rest
	struct aliasNode *prevNode = head;
	struct aliasNode *currNode = head->next;
        while (currNode != NULL) {
	  if (!strcmp(args[1], currNode->alias)) {
            free(currNode->alias);
	    for (int i = 0; i < currNode->argc; i++) {
	      free(currNode->args[i]);
	    }
	    free(currNode->args);
	    prevNode->next = currNode->next; //eliminate from link
            free(currNode);
          }
	  prevNode = currNode;
	  currNode = currNode->next;
	}
      }
      i = 0;
      redir = 0;
      if (fd == NULL) { write(STDOUT_FILENO, prompt, strlen(prompt));}
      continue;
    } else { //cycle through alias names and strcmp with args[0]
      struct aliasNode *currNode = head;
      do {
        if (currNode->alias == NULL) {break;} //head not initialized
	if (!strcmp(currNode->alias, args[0])) {
          for (int i = 0; i < currNode->argc + 1; i++) {
	    args[i] = currNode->args[i]; //alias applied to args
	  }
	  break;
	}
	currNode = currNode->next;
      } while (currNode != NULL);
    }

    cpid = fork();
    int status;
    if (!cpid) { //child
      if (redir) {
        close(STDOUT_FILENO);
	open(refile, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
      }
      execv(args[0], args);
      _exit(1); //exec failed
    } else { //parent
      waitpid(cpid, &status, WUNTRACED | WCONTINUED);
    }
    i = 0;
    redir = 0;
    if (fd == NULL) { write(STDOUT_FILENO, prompt, strlen(prompt));}
  }
}


