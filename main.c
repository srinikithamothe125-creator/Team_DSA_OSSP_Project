#include <stdio.h>
#include "monitor.h"

int main()
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

        printf("\n0. Exit\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

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

            case 0:
                printf("\nExiting Linux System Programming Tool...\n");
                return 0;

            default:
                printf("\nInvalid choice. Try again.\n");
        }
    }

    return 0;
}
