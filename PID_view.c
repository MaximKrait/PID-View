#include <unistd.h>
#include <stdio.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

struct process_in_ram
{
    char name[16]; // Process name buffer
    int priority;  // Process priority value
    char state;    // Process state character
    __pid_t pid;   // Process ID
    int ppid;      // Parent Process ID
};

int main(int argv, char *args[])
{
    struct process_in_ram process;

    if (args[1] == 0)
    {
        fprintf(stderr, "No PID!");
        return 1;
    }
    // Convert command line argument to PID integer
    process.pid = atoi(args[1]);

    // Build path to /proc/<PID>/status file
    char *filepath = malloc(32);
    if (filepath == NULL)
    {
        fprintf(stderr, "memory error");
        return 1;
        free(filepath);
    }

    // Format the path to the process status file
    snprintf(filepath, 32, "/proc/%d/status", process.pid);

    // Open the status file for readings
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        fprintf(stderr, "Process not found");
        return 1;
    }

    // Buffer for reading file lines
    size_t size = 70;
    char *line = malloc(size);
    if (line == NULL)
    {
        fprintf(stderr, "memory error");
        free(filepath);
        fclose(file);
        return 0;
    }

    // Read file line by line using getline
    ssize_t chars_read;
    while ((chars_read = getline(&line, &size, file)) != -1)
    {
        if (strncmp(line, "Name:", 5) == 0)
        {
            sscanf(line, "Name:\t%15s", process.name);
        }
        else if (strncmp(line, "State:", 6) == 0)
        {

            sscanf(line, "State:\t%c", &process.state);
        }
        else if (strncmp(line, "PPid:", 5) == 0)
        {
            sscanf(line, "PPid:\t%d", &process.ppid);
        }
    }
    errno = 0;
    // Print extracted process information
    printf("Name process: %s\n", process.name);
    printf("State: %c\n", process.state);
    // Get process priority using system call
    process.priority = getpriority(PRIO_PROCESS, process.pid);

    if (process.priority == -1)
    {
        if (errno != 0)
        {
            fprintf(stderr, "Error getting priority process");
        }
        else
        {
            printf("priority process: %d\n", process.priority);
        }
    }
    if (process.ppid != 0)
    {
        printf("PPid process: %d\n", (int)process.ppid);
    }
    printf("Program version 0.2\n");

    // close file and clean memory
    free(filepath);
    free(line);
    fclose(file);
    return 0;
}
