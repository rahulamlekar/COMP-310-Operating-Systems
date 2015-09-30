#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define HISTORY_SIZE 10
#define TEXT_INPUT_SIZE 20
#define PROCESS_LIST_SIZE 6

typedef struct command_circular_buffer {
	char* commands[10];
	int index;
	int size;
	int totalCount;
} command_circular_buffer;

typedef struct pid_circular_buffer {
	pid_t pids[PROCESS_LIST_SIZE];
	int index;
} pid_circular_buffer;


int modular_increment(int i, int size) {
    return (i + size + 1) % size;
}


int modular_decriment(int i, int size) {
    return (i + size - 1) % size;
}

/**
 * Push a command to the command buffer.
 */
void command_buffer_push(command_circular_buffer* buffer, char* command) {
	// Decrement the buffer index
	buffer->index = modular_decriment(buffer->index, buffer->size);
	// Delete the current member
	//free(buffer->commands[buffer->index]);
	// Copy the new string string
	char* newCommand = (char *) malloc(sizeof(char) * (TEXT_INPUT_SIZE + 1));
	strcpy(newCommand, command);
	buffer->commands[buffer->index] = newCommand;
	// Increase the total count
	buffer->totalCount++;
}


/**
 * Scan the command buffer for the first command beginning wit the given character.
 */
char* command_buffer_scan(command_circular_buffer buffer, char c) {
	int scanIndex = buffer.index;
	int i;
	for (i = 0; i < buffer.size; i++) {
		// If the command starts with the string
		if (buffer.commands[scanIndex][0] == c) {
			return buffer.commands[scanIndex];
		}
		// Loop through buffer
		scanIndex = modular_decriment(scanIndex, buffer.size);
	}
	// It's not there, so return null
	return NULL;
}


/**
 * Print out the 10 most recent commands
 */
void print_history(command_circular_buffer buffer) {
	printf("Command history (10 most recent of %d total):\n", buffer.totalCount);
	int modularIndex = buffer.index;
	int i;
	for (i = 0; i < HISTORY_SIZE; i++) {
		if (i < buffer.totalCount) {
			int absoluteIndex = buffer.totalCount - i;
			printf("%d: %s\n", absoluteIndex, buffer.commands[modularIndex]);
			modularIndex = modular_increment(modularIndex, buffer.size);
		} else {
			// Fewer elements in buffer than total size.
			return;
		}
	}
}


/**
 * Remove completed processes from the process buffer.
 */
void flush_completed_processes(pid_circular_buffer* buffer) {
	int i;
	for (i = 0; i < PROCESS_LIST_SIZE; i++) {
		pid_t pid = buffer->pids[i];
		int processStatus;
		int test = waitpid(pid, &processStatus, WNOHANG, WUNTRACED);
		printf("Test %d: %d %d\n", pid, test, processStatus);
	}
}


/**
 * Print the list of processes currently running in the buffer.
 */
void print_process_list(pid_circular_buffer buffer) {
	printf("Jobs currently running:\n");
	int totalJobs = 0;
	int i;
	for (i = 0; i < PROCESS_LIST_SIZE; i++) {
		if (buffer.pids[i] > 0) {
			printf("Process %d.\n", buffer.pids[i]);
			totalJobs++;
		}
	}
	if (!totalJobs) {
		printf("None.\n");
	}
}


/**
 * Add a process to the buffer.
 */
void process_list_push(pid_circular_buffer* buffer, pid_t pid) {
	int currentIndex = buffer->index;
	int i;
	for (i = 0; i < PROCESS_LIST_SIZE; i++) {
		if (buffer->pids[currentIndex] == 0) {
			// Add the new pid
			buffer->pids[currentIndex] = pid;
			return;
		}
		currentIndex = modular_increment(currentIndex, PROCESS_LIST_SIZE);
	}
	// There are no free spots, so we will force push one
	buffer->index = modular_increment(buffer->index, PROCESS_LIST_SIZE);
	buffer->pids[buffer->index] = pid;
	printf("Process %d running.\n", pid);
}


/**
 * Load a string array with null strings.
 */
void loadArrayNull(char* array[], int size) {
    int i;
    for (i = 0; i < size; i++) {
        array[i] = NULL;
    }
}


/**
 * Parse the command to extract the argument array.
 */
int parseCmd(char* line, char *args[]) {
    loadArrayNull(args, TEXT_INPUT_SIZE);
    char *token;
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


/**
 * Prompt the user to enter a command.
 */ 
int getcmd(char *prompt, char *args[], int *background, char* newline)
{
    int length;
    char *loc;
    char *line;
    size_t linecap = 0;

    printf("%s", prompt);
    length = getline(&line, &linecap, stdin);

    // Copy the string out
    strcpy(newline, line);

    if (length <= 0) {
        exit(-1);
    }

    // Check if background is specified..
    if ((loc = index(line, '&')) != NULL) {
        *background = 1;
        *loc = ' ';
    } else
        *background = 0;

    // Parse the command to extract the argument array
    return parseCmd(line, args);
}


void freecmd(char* args[], pid_circular_buffer* processes, int bg) {
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
		if (args[1]) {
			// Switch to a job
			pid_t pid = atoi(args[1]);
			printf("Switching to process %d.\n", pid);
			if (!waitpid(pid)) {
				printf("Error switching to process %d.\n", pid);
			}
		} else {
			printf("Please provide a process id argument for \"fg\"\n");
		}
	} else if (strcmp(commandName, "jobs") == 0) {
		// Print the list of current jobs
    	flush_completed_processes(processes);
    	print_process_list(*processes);
	} else {
		// Run a normal command
        pid_t childProcessId = fork();
    	if (!childProcessId) {
    		// This is the child, so execute the command
    		if (execvp(commandName, args) == -1) {
    			printf("Error running command %s.\n", commandName);
    		}
    	} else {
    		// This is the parent, so wait for the child if necessary
            if (bg) {
                // Child executes in async
                // Add to the process list
                process_list_push(processes, childProcessId);
            } else {
                // Child executes synchronously, so we wait
               waitpid(childProcessId);
            }
    	}
	}
}


int main()
{
	// The history of commands
	command_circular_buffer command_history = {
			{"", "", "", "", "", "", "", "", "", ""},
			0,
			10,
			0
	};

	pid_circular_buffer jobs = {
			{0, 0, 0, 0, 0, 0},
			0
	};
    char *args[20];
    int bg;
    char newline[20];

    while (1) {
    	// Reset args
    	loadArrayNull(args, 20);

    	int cnt = getcmd("\n>>  ", args, &bg, newline);

        // Exit if user enters "exit" command
        if (strcmp(args[0], "exit") == 0) {
        	exit(-1);
        }

        // Handle history!
        if (strcmp(args[0], "history") == 0) {
        	// Print out the history
        	print_history(command_history);
        	command_buffer_push(&command_history, newline);
        } else if (strcmp(args[0], "r") == 0) {
        	// We are using the history command!
        	if (!args[1]) {
        		// No command given
        		printf("Please provide a character argument for the 'r' command.");
        	} else {
            	// The char we are  searching for
            	char character = args[1][0];

            	printf("\nHistory for %c!\n", character);
            	char* historicCommand = command_buffer_scan(command_history, character);
            	if (historicCommand == NULL) {
            		printf("NO MATCHING COMMAND IN HISTORY.\n");
            	} else {
            		// There is a matching command!  Add it to the history again.
            		command_buffer_push(&command_history, historicCommand);
            		printf("command: %s", historicCommand);
            		// Extract arguments from the historic command
            		parseCmd(historicCommand, args);
            	}
        	}
        } else {
        	// We're not using history, so add the input to the buffer
        	command_buffer_push(&command_history, newline);
        }

        // Run a command
		freecmd(args, &jobs, bg);

        printf("\n");
    }
}

