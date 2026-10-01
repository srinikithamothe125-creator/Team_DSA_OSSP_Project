#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "monitor.h"

/* CO-2: Process Creation */
void process_creation()
{
    pid_t pid;

    printf("\n========== PROCESS CREATION ==========\n");

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        printf("\n--- Child Process ---\n");
        printf("Child PID  : %d\n", getpid());
        printf("Parent PID : %d\n", getppid());
        printf("Child process is running...\n");
        printf("Child process is terminating.\n");
        exit(0);
    }
    else
    {
        printf("\n--- Parent Process ---\n");
        printf("Parent PID : %d\n", getpid());
        printf("Created Child PID : %d\n", pid);

        wait(NULL);

        printf("Parent: Child process completed.\n");
    }
}

/* CO-2: Process Execution using exec() */
void process_execution()
{
    pid_t pid;

    printf("\n========== PROCESS EXECUTION ==========\n");

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        printf("Child PID: %d\n", getpid());
        printf("Executing another program using exec()...\n");

        execlp("echo",
               "echo",
               "Child successfully executed a new program!",
               NULL);

        perror("exec");
        exit(1);
    }
    else
    {
        wait(NULL);
        printf("Parent: execution completed.\n");
    }
}

/* CO-2: Process Synchronization using wait() */
void process_wait()
{
    pid_t pid;
    int status;

    printf("\n========== PROCESS SYNCHRONIZATION ==========\n");

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        printf("Child PID: %d\n", getpid());
        printf("Child is performing work...\n");

        sleep(2);

        printf("Child process finished.\n");
        exit(5);
    }
    else
    {
        printf("Parent PID: %d\n", getpid());
        printf("Parent is waiting for child...\n");

        wait(&status);

        if (WIFEXITED(status))
        {
            printf("Child terminated normally.\n");
            printf("Child exit status: %d\n",
                   WEXITSTATUS(status));
        }
    }
}
