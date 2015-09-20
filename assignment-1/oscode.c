#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


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
    char *args[20];
    int bg;

    while (1) {
        int cnt = getcmd("\n>>  ", args, &bg);
        int i;
        for (i = 0; i < cnt; i++)
            printf("\nArg[%d] = %s", i, args[i]);

        // Create a new array representing only the arguments for the file we will be running
        char* argumentArray[cnt];
        for (i = 0; i < cnt; i++) {
        	argumentArray[i] = args[i + 1];
        }
        // Terminate with null
        argumentArray[cnt - 1] = NULL;

        int childProcessId = fork();
    	if (!childProcessId) {
    		// This is the child, so execute the command
    		execvp(args[0], args);
    	} else {
    		// This is the parent, so wait for the child if necessary
            if (bg) {
                printf("\nBackground enabled..\n");
                // Child executes in asynch
            } else {
                printf("\nBackground not enabled \n");
                // Child executes synchronously, so we wait
                waitpid(childProcessId);
            }
    	}

        printf("\n\n");

    }

}
