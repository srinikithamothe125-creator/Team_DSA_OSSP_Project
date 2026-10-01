#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/statvfs.h>
#include "monitor.h"

void system_info() {
    printf("\n===== SYSTEM INFORMATION =====\n");

    printf("Process ID : %d\n", getpid());
    printf("Parent PID : %d\n", getppid());

    system("uname -a");
}

void cpu_info() {
    int fd;
    char buffer[4096];
    ssize_t n;

    printf("\n===== CPU INFORMATION =====\n");

    fd = open("/proc/cpuinfo", O_RDONLY);

    if (fd < 0) {
        perror("open");
        return;
    }

    n = read(fd, buffer, sizeof(buffer) - 1);

    if (n > 0) {
        buffer[n] = '\0';
        printf("%.1500s\n", buffer);
    }

    close(fd);
}

void memory_info() {
    FILE *file;
    char line[256];

    printf("\n===== MEMORY INFORMATION =====\n");

    file = fopen("/proc/meminfo", "r");

    if (file == NULL) {
        perror("fopen");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "MemTotal", 8) == 0 ||
            strncmp(line, "MemFree", 7) == 0 ||
            strncmp(line, "MemAvailable", 12) == 0) {

            printf("%s", line);
        }
    }

    fclose(file);
}

void disk_info() {
    struct statvfs disk;

    printf("\n===== DISK INFORMATION =====\n");

    if (statvfs("/", &disk) != 0) {
        perror("statvfs");
        return;
    }

    unsigned long long total =
        (unsigned long long)disk.f_blocks * disk.f_frsize;

    unsigned long long free =
        (unsigned long long)disk.f_bfree * disk.f_frsize;

    printf("Total Disk : %llu MB\n",
           total / (1024 * 1024));

    printf("Free Disk  : %llu MB\n",
           free / (1024 * 1024));
}

void uptime_info() {
    FILE *file;
    double uptime;

    printf("\n===== SYSTEM UPTIME =====\n");

    file = fopen("/proc/uptime", "r");

    if (file == NULL) {
        perror("fopen");
        return;
    }

    fscanf(file, "%lf", &uptime);

    printf("System Uptime: %.2f seconds\n", uptime);

    fclose(file);
}
