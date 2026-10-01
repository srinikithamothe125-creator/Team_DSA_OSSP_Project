#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <stdlib.h>
#include <string.h>
#include "monitor.h"

/* CO-1: System Information */
void system_info()
{
    struct utsname info;

    printf("\n========== SYSTEM INFORMATION ==========\n");

    if (uname(&info) == -1)
    {
        perror("uname");
        return;
    }

    printf("Operating System : %s\n", info.sysname);
    printf("Kernel Release   : %s\n", info.release);
    printf("Kernel Version   : %s\n", info.version);
    printf("Machine          : %s\n", info.machine);

    printf("\nCurrent Process ID : %d\n", getpid());
    printf("Parent Process ID  : %d\n", getppid());
}

/* CO-1: System Call Demonstration */
void syscall_demo()
{
    printf("\n========== SYSTEM CALL DEMONSTRATION ==========\n");

    printf("Process ID  : %d\n", getpid());
    printf("Parent PID  : %d\n", getppid());
    printf("User ID     : %d\n", getuid());
    printf("Group ID    : %d\n", getgid());

    printf("\nSystem calls executed successfully.\n");
}

/* CO-1: File I/O using open/read/write/close */
void file_io_demo()
{
    int fd;
    char message[] = "Linux System Programming - File I/O Demo\n";
    char buffer[100];
    int bytes;

    printf("\n========== FILE I/O USING SYSTEM CALLS ==========\n");

    fd = open("monitor_data.txt",
              O_CREAT | O_RDWR | O_TRUNC, 0644);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    printf("File opened successfully.\n");

    write(fd, message, strlen(message));
    printf("Data written using write().\n");

    lseek(fd, 0, SEEK_SET);

    bytes = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes == -1)
    {
        perror("read");
        close(fd);
        return;
    }

    buffer[bytes] = '\0';

    printf("Data read using read():\n");
    printf("%s", buffer);

    close(fd);

    printf("File closed using close().\n");
}

/* CO-1: Shell / command execution */
void shell_demo()
{
    pid_t pid;

    printf("\n========== SHELL COMMAND EXECUTION ==========\n");

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        printf("Child process executing shell command...\n");

        execl("/bin/sh",
              "sh",
              "-c",
              "uname -a",
              NULL);

        perror("execl");
        exit(1);
    }
    else
    {
        wait(NULL);
        printf("Parent: shell command completed.\n");
    }
}
