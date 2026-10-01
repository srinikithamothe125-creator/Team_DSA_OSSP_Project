#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include "monitor.h"

int pipe_fd[2];

void pipe_demo() {
    pid_t pid;

    char message[] = "Hello from Parent";
    char buffer[100];

    printf("\n===== PIPE IPC =====\n");

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return;
    }

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {

        close(pipe_fd[1]);

        read(pipe_fd[0],
             buffer,
             sizeof(buffer));

        printf("Child received: %s\n", buffer);

        close(pipe_fd[0]);
    }
    else {

        close(pipe_fd[0]);

        write(pipe_fd[1],
              message,
              strlen(message) + 1);

        close(pipe_fd[1]);

        wait(NULL);
    }
}

void signal_handler(int signal_number) {

    printf("\nSignal received: %d\n",
           signal_number);

    printf("Signal handler executed!\n");
}

void signal_demo() {

    printf("\n===== SIGNAL DEMO =====\n");

    signal(SIGINT, signal_handler);

    printf("Process PID: %d\n", getpid());

    printf("Press Ctrl+C to send SIGINT.\n");

    sleep(5);

    signal(SIGINT, SIG_DFL);
}
