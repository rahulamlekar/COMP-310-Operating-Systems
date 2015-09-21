#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define PROCESS_LIST_SIZE 6

typedef struct command_circular_buffer {
	char commands[10][20];
	int index = 0;
	int size = 20;
} command_circular_buffer;

void circular_buffer_push(char* command[20], command_circular_buffer buffer) {
	// Decrement the buffer index
	buffer.index = (buffer.index + buffer.size - 1) % buffer.size;
	// Copy the new string string
	strcpy(buffer.commands[buffer.index], command);
}

char* circular_buffer_pop(command_circular_buffer buffer) {
	// The string to output
	char output[] = buffer.commands[buffer.index];
	// Increment the index
	buffer.index = (buffer.index + buffer.size + 1) % buffer.size;
	// Return the string
	return output;
}

void flush_completed_processes(pid_t* processes, int n) {
	int i;
	for (i = 0; i < n; i++) {
		// If the process is complete, remove it from the list
		if (kill(*processes[i], 2) == -1) {
			*processes[i] = -1;
		}
	}
}

void print_process_list(pid_t processes, n) {
	int i;
	for (i = 0; i < n; i++) {
		if (processes[i] >= 0) {
			printf("Process %d running.\n", processes[i]);
		}
	}
}

int getcmd(char *prompt, char *args[], int *background)
{
    int length, i = 0;
    char *token, *loc;
    char *line;
    size_t linecap = 0;

    printf("%s", prompt);
    length = getline(&line, &linecap, stdin);

    if (length <= 0) {
        exit(-1);
    }

    // Check if background is specified..
    if ((loc = index(line, '&')) != NULL) {
        *background = 1;
        *loc = ' ';
    } else
        *background = 0;

    while ((token = strsep(&line, " \t\n")) != NULL) {
    	int j;
        for (j = 0; j < strlen(token); j++)
            if (token[j] <= 32)
                token[j] = '\0';
        if (strlen(token) > 0)
            args[i++] = token;
    }

    return i;
}


int main()
{
	command_circular_buffer command_history;
	pid_t processes[] = {-1, -1, -1, -1, -1, -1};
    char *args[20];
    int bg;

    while (1) {
    	flush_completed_processes(&processes, PROCESS_LIST_SIZE);

    	int cnt = getcmd("\n>>  ", args, &bg);
        int i;
        for (i = 0; i < cnt; i++)
            printf("\nArg[%d] = %s", i, args[i]);

        // If we are using a baked command, continue
        if (freecmd(args)) {
        	continue;
        }

//        // Create a new array representing only the arguments for the file we will be running
//        char* argumentArray[cnt];
//        for (i = 0; i < cnt; i++) {
//        	argumentArray[i] = args[i + 1];
//        }
//        // Terminate with null
//        argumentArray[cnt - 1] = NULL;
//
//        pid_t childProcessId = fork();
//    	if (!childProcessId) {
//    		// This is the child, so execute the command
//    		execvp(args[0], args);
//    	} else {
//    		// This is the parent, so wait for the child if necessary
//            if (bg) {
//                printf("\nBackground enabled..\n");
//                // Child executes in asynch
//            } else {
//                printf("\nBackground not enabled \n");
//                // Child executes synchronously, so we wait
//                waitpid(childProcessId);
//            }
//    	}

        printf("\n\n");
    }
}


int freecmd(char* args[], pid_t processes[]) {
	// Get the name of the command we will run
	char* commandName = args[0];

	// If the command is one of the default commands, run that
	if (strcmp(commandName, "cd") == 0) {
		// Change the current directory to the first argument
		chdir(args[1]);
		return 1;
	} else if (strcmp(commandName, "pwd") == 0) {
		char pwd[256];
		printf("%s", getcwd(pwd, sizeof(pwd)));
		return 1;
	} else if (strcmp(commandName, "fg") == 0) {
		return 1;
	} else if (strcmp(commandName, "jobs") == 0) {
    	print_process_list(processes, PROCESS_LIST_SIZE);
    	return 1;
	} else if (strcmp(commandName, "exit") == 0) {
		// Exit the terminal
		exit(-1);
	}

	return 0;
}

