#include <unistd.h>
#include <stdio.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>

struct inspector_ram_2v
{
    char name[16]; // Process name buffer
    int priority;  // Process priority value
    char state;    // Process state character
    __pid_t pid;   // Process ID
};

int main(int argv, char *args[])
{
    struct inspector_ram_2v process;

    // Convert command line argument to PID integer
    process.pid = atoi(args[1]);

    // Build path to /proc/<PID>/status file
    char filepath[64];
    snprintf(filepath, sizeof(filepath), "/proc/%d/status", process.pid);

    // Open the status file for readings
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        printf("File not opened!");
        return 1;
    }

    // Buffer for reading file lines
    char line[256];

    // Read file line by line
    while (fgets(line, sizeof(line), file))
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
    printf("Program version 0.1");

    // close file
    fclose(file);
    return 0;
}
