#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define PROCESS_LIST_SIZE 6

typedef struct command_circular_buffer {
	char* commands[10];
	int index;
	int size;
} command_circular_buffer;

void circular_buffer_push(command_circular_buffer buffer, char* command) {
	// Decrement the buffer index
	buffer.index = modular_decriment(buffer.index, buffer.size);
	// Copy the new string string
	strcpy(buffer.commands[buffer.index], command);
}

char* circular_buffer_pop(command_circular_buffer buffer) {
	int oldIndex = buffer.index;
	// Increment the index
	buffer.index = modular_increment(buffer.index, buffer.size);
	// Return the string
	return buffer.commands[buffer.index];
}

char* circular_buffer_scan(command_circular_buffer buffer, char c) {
	int scanIndex = buffer.index;
	int i;
	for (i = 0; i < buffer.size; i++) {
		// If the command starts with the string
		if (buffer.commands[buffer.index][0] == c) {
			return buffer.commands[buffer.index];
		}
		// Loop through buffer
		scanIndex = modular_decriment(scanIndex, buffer.size);
	}
	// It's not there, so return null
	return NULL;
}

void circular_buffer_replace(command_circular_buffer buffer, char* command) {
	strcpy(buffer.commands[buffer.index], command);
}

int modular_increment(int i, int size) {
	return (i + size + 1) % size;
}

int modular_decriment(int i, int size) {
	return (i + size - 1) % size;
}

//void flush_completed_processes(pid_t processes[], int n) {
//	int i;
//	for (i = 0; i < n; i++) {
//		// If the process is complete, remove it from the list
//		if (kill(*processes[i], 2) == -1) {
//			*processes[i] = -1;
//		}
//	}
//}
//
//void print_process_list(pid_t processes, n) {
//	int i;
//	for (i = 0; i < n; i++) {
//		if (processes[i] >= 0) {
//			printf("Process %d running.\n", processes[i]);
//		}
//	}
//}

int getcmd(char *prompt, char *args[], int *background, char* newline)
{
    int length;
    char *token, *loc;
    char *line;
    size_t linecap = 0;

    printf("%s", prompt);
    length = getline(&line, &linecap, stdin);

    strcpy(newline, line);


    printf("\n\n %s \n\n", line);

    if (length <= 0) {
        exit(-1);
    }

    // Check if background is specified..
    if ((loc = index(line, '&')) != NULL) {
        *background = 1;
        *loc = ' ';
    } else
        *background = 0;

    int i = 0;
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

//void tokenize_cmd(char* args[], char* line) {
//
//}


int main()
{
	// The history of commands
	command_circular_buffer command_history = {
			{"", "", "", "", "", "", "", "", "", ""},
			0,
			10

	};

	pid_t processes[] = {-1, -1, -1, -1, -1, -1};
    char *args[20];
    int bg;
    char* newline;

    while (1) {
    	//flush_completed_processes(&processes, PROCESS_LIST_SIZE);

    	int cnt = getcmd("\n>>  ", args, &bg, newline);
        int i;
        for (i = 0; i < cnt; i++)
            printf("\nArg[%d] = %s", i, args[i]);

        // Exit if user enters "exit" command
        if (strcmp(args[0], "exit") == 0) {
        	exit(-1);
        }

        pid_t childProcessId = fork();
    	if (!childProcessId) {
    		// This is the child, so execute the command
    		freecmd(args);
    	} else {
    		// This is the parent, so wait for the child if necessary
            if (bg) {
                printf("\nBackground enabled..\n");
                // Child executes in async
            } else {
                printf("\nBackground not enabled \n");
                // Child executes synchronously, so we wait
                waitpid(childProcessId);
            }
    	}

        printf("\n\n");
    }
}


freecmd(char* args[], pid_t processes[]) {
	// Get the name of the command we will run
	char* commandName = args[0];

	// If the command is one of the default commands, run that
	if (strcmp(commandName, "cd") == 0) {
		// Change the current directory to the first argument
		chdir(args[1]);
	} else if (strcmp(commandName, "pwd") == 0) {
		// Print the current directory
		char pwd[256];
		printf("%s", getcwd(pwd, sizeof(pwd)));
	} else if (strcmp(commandName, "fg") == 0) {
		// TODO: Switch to a job

	} else if (strcmp(commandName, "jobs") == 0) {
		// Print the list of current jobs
    	//print_process_list(processes, PROCESS_LIST_SIZE);
	} else {
		// Run a normal command
		execvp(commandName, args);
	}

	return 0;
}

