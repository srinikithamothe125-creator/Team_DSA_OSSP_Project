#include <stdio.h>
#include "monitor.h"

int main(void)
{
    int choice;

    while (1)
    {
        printf("\n============================================\n");
        printf("       LINUX SYSTEM PROGRAMMING TOOL\n");
        printf("============================================\n");

        printf("\n--- CO-1 : SYSTEM CALLS & OS SERVICES ---\n");
        printf("1. System Information\n");
        printf("2. System Call Demonstration\n");
        printf("3. File I/O using System Calls\n");
        printf("4. Shell Command Execution\n");

        printf("\n--- CO-2 : PROCESS MANAGEMENT ---\n");
        printf("5. Create Parent-Child Process\n");
        printf("6. Process Execution using exec()\n");
        printf("7. Process Synchronization using wait()\n");

        printf("\n--- CO-3 : INTER-PROCESS COMMUNICATION ---\n");
        printf("8. Anonymous Pipe\n");
        printf("9. Named Pipe (FIFO)\n");
        printf("10. Signal Communication\n");

        printf("\n--- CO-4 : MEMORY MANAGEMENT ---\n");
        printf("11. Dynamic Memory Allocation and Memory Mapping\n");

        printf("\n--- CO-5 : FILE SYSTEM OPERATIONS ---\n");
        printf("12. File Operations and Metadata\n");

        printf("\n--- CO-6 : MULTITHREADING & SYNCHRONIZATION ---\n");
        printf("13. Thread and Semaphore Demonstration\n");

        printf("\n0. Exit\n");

        printf("\nEnter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("\nInvalid input. Please enter a number.\n");

            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
                /* Clear invalid input */
            }

            continue;
        }

        switch (choice)
        {
            case 1:
                system_info();
                break;

            case 2:
                syscall_demo();
                break;

            case 3:
                file_io_demo();
                break;

            case 4:
                shell_demo();
                break;

            case 5:
                process_creation();
                break;

            case 6:
                process_execution();
                break;

            case 7:
                process_wait();
                break;

            case 8:
                pipe_demo();
                break;

            case 9:
                fifo_demo();
                break;

            case 10:
                signal_demo();
                break;

            case 11:
                memory_demo();
                break;

            case 12:
                filesystem_demo();
                break;

            case 13:
                thread_demo();
                break;

            case 0:
                printf("\nExiting LinuxMonitor...\n");
                return 0;

            default:
                printf("\nInvalid choice. Try again.\n");
        }
    }

    return 0;
}