#pragma once
#include <sys/types.h>

struct process_in_ram {
    char name[16];
    int priority;
    char state;
    pid_t pid;
    int ppid;
    int threads;
    char uid_name[32];
    long vmem_mb;
    long rss_mb;
    long uptime_sec;
};

void parse_proc_status(const char *buffer, struct process_in_ram *proc);
void parse_advanced_metrics(struct process_in_ram *proc);