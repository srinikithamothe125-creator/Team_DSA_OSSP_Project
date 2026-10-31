#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

void memory_demo(void)
{
    printf("\n========== CO4: MEMORY MANAGEMENT ==========\n");

    int *numbers = malloc(5 * sizeof(int));

    if (numbers == NULL)
    {
        perror("malloc");
        return;
    }

    printf("\n1. Dynamic Memory Allocation using malloc()\n");

    for (int i = 0; i < 5; i++)
    {
        numbers[i] = (i + 1) * 10;
    }

    printf("Allocated values: ");

    for (int i = 0; i < 5; i++)
    {
        printf("%d ", numbers[i]);
    }

    printf("\n");

    free(numbers);
    printf("Memory released using free().\n");

    int *mapped = mmap(
        NULL,
        5 * sizeof(int),
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (mapped == MAP_FAILED)
    {
        perror("mmap");
        return;
    }

    printf("\n2. Memory Mapping using mmap()\n");

    for (int i = 0; i < 5; i++)
    {
        mapped[i] = (i + 1) * 100;
    }

    printf("Mapped values: ");

    for (int i = 0; i < 5; i++)
    {
        printf("%d ", mapped[i]);
    }

    printf("\n");

    if (munmap(mapped, 5 * sizeof(int)) == -1)
    {
        perror("munmap");
        return;
    }

    printf("Mapped memory released using munmap().\n");
    printf("===========================================\n");
}