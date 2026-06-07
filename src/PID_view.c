#include <unistd.h>
#include <stdio.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pwd.h>

struct process_in_ram {
    char name[16];
    int priority;
    char state;
    pid_t pid;
    int ppid;
    int threads;
    char uid_name[32];
};

void parse_proc_status(const char *buffer, struct process_in_ram *proc) {
    const char *line = buffer;
    int raw_uid = -1;

    while (*line) {
        if (strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "Name:\t%15s", proc->name);
        } else if (strncmp(line, "State:", 6) == 0) {
            sscanf(line, "State:\t%c", &proc->state);
        } else if (strncmp(line, "PPid:", 5) == 0) {
            sscanf(line, "PPid:\t%d", &proc->ppid);
        } else if (strncmp(line, "Threads:", 8) == 0) {
            sscanf(line, "Threads:\t%d", &proc->threads);
        } else if (strncmp(line, "Uid:", 4) == 0) {
            sscanf(line, "Uid:\t%d", &raw_uid);
        }

        while (*line && *line != '\n') line++;
        if (*line == '\n') line++;
    }

    if (raw_uid != -1) {
        struct passwd *pw = getpwuid(raw_uid);
        if (pw) {
            strncpy(proc->uid_name, pw->pw_name, sizeof(proc->uid_name) - 1);
        } else {
            snprintf(proc->uid_name, sizeof(proc->uid_name), "%d", raw_uid);
        }
    } else {
        strcpy(proc->uid_name, "unknown");
    }
}

int main(int argv, char *args[]) {
    if (argv < 2) {
        fprintf(stderr, "Usage: %s <PID>\n", args[0]);
        return 1;
    }

    struct process_in_ram process = {0};
    process.pid = atoi(args[1]);
    char filepath[32];
    char buffer[1024];

    snprintf(filepath, sizeof(filepath), "/proc/%d/status", process.pid);

    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Process %d not found\n", process.pid);
        return 1;
    }

    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    if (bytes_read <= 0) {
        fprintf(stderr, "Failed to read status\n");
        return 1;
    }
    buffer[bytes_read] = '\0';

    parse_proc_status(buffer, &process);

    printf("=== PID-View v0.4 ===\n");
    printf("PID:          %d\n", process.pid);
    printf("Name:         %s\n", process.name);
    printf("Owner:        %s\n", process.uid_name);
    printf("State:        %c\n", process.state);

    errno = 0;
    process.priority = getpriority(PRIO_PROCESS, process.pid);
    if (process.priority == -1 && errno != 0) {
        fprintf(stderr, "Error getting priority process\n");
        return 1;
    }

    printf("Priority:     %d\n", process.priority);
    if (process.ppid != 0) {
        printf("Parent PID:   %d\n", process.ppid);
    }
    printf("Threads:      %d\n", process.threads);
    printf("=====================\n");
    
    return 0;
}
