#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "monitor.h"

/* CO-3: Anonymous Pipe */
void pipe_demo()
{
    int pipefd[2];
    pid_t pid;
    char message[] = "Hello from Parent through Pipe!";
    char buffer[100];

    printf("\n========== ANONYMOUS PIPE ==========\n");

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return;
    }

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        close(pipefd[1]);

        read(pipefd[0], buffer, sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';

        printf("Child PID : %d\n", getpid());
        printf("Message received by child: %s\n", buffer);

        close(pipefd[0]);
    }
    else
    {
        close(pipefd[0]);

        write(pipefd[1], message, strlen(message) + 1);
        close(pipefd[1]);

        wait(NULL);

        printf("Parent PID : %d\n", getpid());
        printf("Message sent successfully through pipe.\n");
    }
}

/* CO-3: Named Pipe / FIFO */
void fifo_demo()
{
    const char *fifo_name = "monitor_fifo";
    char message[] = "Message sent through Named Pipe!";
    char buffer[100];
    int fd;

    printf("\n========== NAMED PIPE (FIFO) ==========\n");

    if (mkfifo(fifo_name, 0666) == -1)
    {
        /* FIFO may already exist */
        printf("Using existing FIFO.\n");
    }
    else
    {
        printf("FIFO created successfully.\n");
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        fd = open(fifo_name, O_RDONLY);

        if (fd == -1)
        {
            perror("open FIFO");
            exit(1);
        }

        read(fd, buffer, sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';

        printf("Child received: %s\n", buffer);

        close(fd);
        exit(0);
    }
    else
    {
        sleep(1);

        fd = open(fifo_name, O_WRONLY);

        if (fd == -1)
        {
            perror("open FIFO");
            return;
        }

        write(fd, message, strlen(message) + 1);
        close(fd);

        wait(NULL);

        unlink(fifo_name);

        printf("Parent sent message through FIFO.\n");
        printf("FIFO removed successfully.\n");
    }
}

/* Signal handler */
void signal_handler(int signal_number)
{
    printf("\nSignal received: %d\n", signal_number);
    printf("Signal handler executed successfully.\n");
}

/* CO-3: Signal Communication */
void signal_demo()
{
    pid_t pid;

    printf("\n========== SIGNAL COMMUNICATION ==========\n");

    signal(SIGUSR1, signal_handler);

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        sleep(1);

        printf("Child PID %d sending SIGUSR1...\n",
               getpid());

        kill(getppid(), SIGUSR1);

        exit(0);
    }
    else
    {
        printf("Parent PID: %d\n", getpid());
        printf("Waiting for signal from child...\n");

        pause();

        wait(NULL);

        printf("Parent: signal communication completed.\n");
    }
}
