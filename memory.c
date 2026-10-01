#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "monitor.h"

void memory_demo() {

    printf("\n===== MEMORY MANAGEMENT =====\n");

    int *data;

    data = malloc(5 * sizeof(int));

    if (data == NULL) {
        perror("malloc");
        return;
    }

    for (int i = 0; i < 5; i++) {
        data[i] = (i + 1) * 10;
    }

    printf("Dynamic Memory Values:\n");

    for (int i = 0; i < 5; i++) {
        printf("%d ", data[i]);
    }

    printf("\n");

    free(data);

    printf("\nProcess Memory Map:\n");

    char command[100];

    snprintf(command,
             sizeof(command),
             "cat /proc/%d/maps | head -10",
             getpid());

    system(command);

    printf("\nUsing mmap():\n");

    void *memory = mmap(
        NULL,
        4096,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (memory == MAP_FAILED) {
        perror("mmap");
        return;
    }

    strcpy((char *)memory,
           "Memory mapped successfully!");

    printf("%s\n", (char *)memory);

    munmap(memory, 4096);
}
