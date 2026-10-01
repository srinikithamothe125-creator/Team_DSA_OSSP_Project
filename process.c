#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "monitor.h"

void process_info() {
    printf("\n===== PROCESS INFORMATION =====\n");

    printf("Current Process PID : %d\n", getpid());
    printf("Parent Process PID  : %d\n", getppid());

    printf("\nRunning Processes:\n");

    system("ps -eo pid,ppid,comm,%mem --sort=-%mem | head -15");
}

void process_creation() {
    pid_t pid;

    printf("\n===== PROCESS CREATION =====\n");

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        printf("Child Process\n");
        printf("Child PID: %d\n", getpid());

        execlp("echo",
               "echo",
               "Child process executed using exec()",
               NULL);

        perror("exec");
        exit(1);
    }
    else {
        printf("Parent Process\n");
        printf("Parent PID: %d\n", getpid());

        wait(NULL);

        printf("Child process completed.\n");
    }
}
