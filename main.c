#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */

void copyarg(const char *arg, char *args[10][MAX_LINE/2 + 1], int i, int j) /* copies a pointer to arg to args[] */
{
  char *copy = (char *)malloc(strlen(arg) + 1); /* allocate memory for arg */
  strcpy(copy, arg);
  args[i][j] = copy;
}

void exec(const char *command, char *args[]) /* executes child process */
{
  execvp(command, args);
	/* will only execute if execvp() fails */
	printf("Unknown command\n"); 
	exit(0);
}

int main(void)
{
  char *args[11][MAX_LINE/2 + 1]; /* command line arguments */
  int should_run = 1; /* flag to determine when to exit program */
  int i = 0; /* args[i][]: # of line in history */

  while (should_run)
  {
	printf("> "); /* print prompt */
	fflush(stdout);

	char arg[MAX_LINE] = {'\0'}; /* current arg being scanned */
	int j = 0; /* args[][j]: index of arg in line */
	while (1) {
	  scanf("%[^ \n]", arg); /* scan in next arg, stopping if a space or newline is reached */
	  if (getchar() == '\n') /* check if last arg in buffer */
	  {
		copyarg(arg, args, i % 11, j);
		args[i % 11][j + 1] = NULL; /* ensure line terminates with NULL */
		break; /* stop scanning if last arg in buffer */
	  }
	  copyarg(arg, args, i % 11, j);
	  j++;
	}

	if (strcmp(args[i % 11][0], "") == 0) /* if no command was entered, skip rest of code */
	{
	  continue;
	}
	else if (strcmp(args[i % 11][0], "exit") == 0) /* if command "exit" was entered, set should_run to 0 and terminate shell */
	{
	  should_run = 0;
	  break;
	}
	else if (strcmp(args[i % 11][0], "history") == 0) /* if command "history" was entered, print history and skip rest of code */
	{
	  args[i % 11][0] = NULL; /* remove "history" command from history */
	  for (int x = i - 1; x >= i - 10 && x >= 0; x--) /* print history */
	  {
		int y = 0; /* reset y to 0 each time a new line is printed */
		printf("%d ", x + 1); /* print number of line in history */
		while (args[x % 11][y]) /* print line x */
		{
		  printf("%s ", args[x % 11][y]); /* print arg y */
		  y++;
		}
		printf("\n");
	  }
	  continue;
	}
	else if (args[i % 11][0][0] == '!') /* if command "!!" or "!n" was entered, copy arg from history to current args line i and execute */
	{
	  if (i == 0) /* if there are no commands in history, print error message and skip rest of code */
	  {
		printf("No commands in history\n");
		continue;
	  }

	  int n = 0; /* number of command in history */
	  if (args[i % 11][0][1] == '!') /* if "!!" was entered, execute last command */
	  {
		n = i - 1; 
	  }
	  else /* if "!n" was entered, execute nth last command */
	  {
		n = atoi(args[i % 11][0] + 1) - 1; 
	  }
	  args[i][0] = NULL; /* remove "!n" from args */

	  if (n < 0 || (n < i - 10 || n >= i)) /* if n is outside of history's range, print error message and skip rest of code */
	  {
		printf("No such command in history\n");
		continue;
	  }

	  j = 0; /* re-step through j so that it matches new length of line i */
	  while (args[n % 11][j]) /* copy line n to current line i */
	  {
		args[i % 11][j] = args[n % 11][j];
		printf("%s ", args[i % 11][j]); /* echo line being copied */
		j++;
	  }
	  args[i % 11][j] = NULL; /* ensure line terminates in NULL */
	  j--;
	  printf("\n");
	}

  	int child_pid; /* child process id */
	int child_status; /* necessary for wait() */
	child_pid = fork();
	i++;
	
	if (strcmp(args[(i - 1) % 11][j], "&") == 0)
	{
	  char *trimmed_args[MAX_LINE/2 + 1]; /* args[i] excluding "&" */
	  j = 0; /* re-step through j so that it matches new length of trimmed line */
	  while (strcmp(args[(i - 1) % 11][j], "&") != 0) /* copy line i to trimmed_args, excluding final arg */
	  {
		trimmed_args[j] = args[(i - 1) % 11][j];
		j++;
	  }
	  trimmed_args[j] = NULL; /* ensure trimmed_args terminates in NULL */

	  if (child_pid == 0) /* execute child process without waiting */
	  {
		exec(args[(i - 1) % 11][0], trimmed_args);
	  }

	}
	else
	{
	  if (child_pid == 0) /* execute child process */
	  {
		exec(args[(i - 1) % 11][0], args[(i - 1) % 11]);
	  }
	  else /* wait for child process to execute, then continue */
	  {
		while (1)
		{
		  int wait_pid = wait(&child_status); /* wait() will return child_pid after it's done executing */
		  if (wait_pid == child_pid)
		  {
			break;
		  }
		}
	  }
	}
  }

  return 0;
}
