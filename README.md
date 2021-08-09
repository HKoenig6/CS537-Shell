UW Madison Spring 2021 Project - Command Shell

Available Functionality:

   -execute bash arguments (uses execvp)

   -basic file redirection using '>' (only one file)

   -aliasing adds new keywords:

      *alias- usage: alias <name> <arg0> <arg1> ... (creates alias name with args) OR
   
      alias <name> (displays alias name's args) OR
   
      alias (displays all saved aliases)
   
      *unalias- usage: unalias <name>
   
      (implemented linked list for aliases)
   
   -'exit' will terminate the shell
   
Type
'''
./mysh
'''
to enter interactive mode OR
'''
./mysh  <filename>
'''
to execute lines from a file in batch mode.
