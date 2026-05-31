#include <unistd.h>
#include <stdio.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>

struct inspector_ram_2v
{
    char name[16];
    int priority;
    char state[256];
    __pid_t pid;
};

int main(int argv, char *args[])
{
    struct inspector_ram_2v process;
    process.pid = atoi(args[1]);
    char filepath[64];
    snprintf(filepath, sizeof(filepath), "/proc/%d/status", process.pid);
    FILE *file = fopen(filepath, "r");
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "Name:", 5) == 0)
        {

            sscanf(line, "Name:\t%15s", process.name);
        }
        else if (strncmp(line, "State:", 6) == 0)
        {

            sscanf(line, "State:\t%c", &process.state[0]);
            process.state[1] = '\0';
        }
    }
    printf("Name process: %s\n", process.name);
    printf("State: %s\n", process.state);
    int priority = getpriority(PRIO_PROCESS, process.pid);
    printf("priority process: %d\n", priority);
    fclose(file);
    return 0;
}
