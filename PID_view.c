#include <unistd.h>
#include <stdio.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>

struct process_in_ram
{
    char name[16]; // Process name buffer
    int priority;  // Process priority value
    char state;    // Process state character
    __pid_t pid;   // Process ID
};

int main(int argv, char *args[])
{
    struct process_in_ram process;

    // Convert command line argument to PID integer
    process.pid = atoi(args[1]);

    // Build path to /proc/<PID>/status file
    char *filepath = malloc(32);
    if (filepath == NULL)
    {
        printf("memory error");
        return 0;
    }
    snprintf(filepath, 32, "/proc/%d/status", process.pid);

    // Open the status file for readings
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        printf("File not opened!");
        return 1;
    }

    // Buffer for reading file lines
    char *line = malloc(70);
    if (line == NULL)
    {
        printf("memory error");
        return 0;
    }

    // Read file line by line
    while (fgets(line, 70, file))
    {
        if (strncmp(line, "Name:", 5) == 0)
        {

            sscanf(line, "Name:\t%15s", process.name);
        }
        else if (strncmp(line, "State:", 6) == 0)
        {

            sscanf(line, "State:\t%c", &process.state);
        }
    }

    // Get process priority using system call
    process.priority = getpriority(PRIO_PROCESS, process.pid);

    // Print extracted process information
    printf("Name process: %s\n", process.name);
    printf("State: %c\n", process.state);
    printf("priority process: %d\n", process.priority);
    printf("Program version 0.1\n");
    free(filepath);
    free(line);

    // close file
    fclose(file);
    return 0;
}
