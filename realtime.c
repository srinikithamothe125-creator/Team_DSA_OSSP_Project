#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/statvfs.h>
#include "monitor.h"

double get_cpu_percent()
{
    FILE *file;
    char line[256];

    unsigned long long user1, nice1, system1, idle1;
    unsigned long long user2, nice2, system2, idle2;

    file = fopen("/proc/stat", "r");

    if (file == NULL)
        return 0.0;

    fgets(line, sizeof(line), file);

    sscanf(line, "cpu %llu %llu %llu %llu",
           &user1, &nice1, &system1, &idle1);

    fclose(file);

    sleep(1);

    file = fopen("/proc/stat", "r");

    if (file == NULL)
        return 0.0;

    fgets(line, sizeof(line), file);

    sscanf(line, "cpu %llu %llu %llu %llu",
           &user2, &nice2, &system2, &idle2);

    fclose(file);

    unsigned long long total1 =
        user1 + nice1 + system1 + idle1;

    unsigned long long total2 =
        user2 + nice2 + system2 + idle2;

    unsigned long long total = total2 - total1;
    unsigned long long idle = idle2 - idle1;

    if (total == 0)
        return 0.0;

    return 100.0 * (1.0 - ((double)idle / total));
}


double get_memory_percent()
{
    FILE *file;
    char line[256];

    unsigned long total = 0;
    unsigned long available = 0;

    file = fopen("/proc/meminfo", "r");

    if (file == NULL)
        return 0.0;

    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "MemTotal: %lu kB", &total) == 1)
            continue;

        if (sscanf(line, "MemAvailable: %lu kB",
                   &available) == 1)
            break;
    }

    fclose(file);

    if (total == 0)
        return 0.0;

    return ((double)(total - available) / total) * 100.0;
}


double get_disk_percent()
{
    struct statvfs disk;

    if (statvfs("/", &disk) != 0)
        return 0.0;

    unsigned long long total =
        (unsigned long long)disk.f_blocks * disk.f_frsize;

    unsigned long long free_space =
        (unsigned long long)disk.f_bfree * disk.f_frsize;

    if (total == 0)
        return 0.0;

    return ((double)(total - free_space) / total) * 100.0;
}


void print_bar(double percent)
{
    int filled = (int)(percent / 5.0);

    if (filled > 20)
        filled = 20;

    printf("[");

    for (int i = 0; i < 20; i++)
    {
        if (i < filled)
            printf("#");
        else
            printf("-");
    }

    printf("] %5.1f%%", percent);
}


void print_uptime()
{
    FILE *file;
    double uptime;

    file = fopen("/proc/uptime", "r");

    if (file == NULL)
        return;

    fscanf(file, "%lf", &uptime);
    fclose(file);

    int hours = (int)uptime / 3600;
    int minutes = ((int)uptime % 3600) / 60;
    int seconds = (int)uptime % 60;

    printf("%02d:%02d:%02d", hours, minutes, seconds);
}


void print_processes()
{
    FILE *file;
    char line[256];

    printf("║                    TOP PROCESSES                       ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ PID      PROCESS             CPU %%       MEMORY %%       ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");

    file = popen(
        "ps -eo pid,comm,%cpu,%mem --sort=-%cpu | head -n 6",
        "r"
    );

    if (file == NULL)
        return;

    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file))
    {
        int pid;
        char name[32];
        float cpu;
        float memory;

        if (sscanf(line, "%d %31s %f %f",
                   &pid, name, &cpu, &memory) == 4)
        {
            printf("║ %-8d %-19s %-11.1f %-14.1f ║\n",
                   pid, name, cpu, memory);
        }
    }

    pclose(file);
}


void realtime_monitor()
{
    while (1)
    {
        double cpu = get_cpu_percent();
        double memory = get_memory_percent();
        double disk = get_disk_percent();

        system("clear");

        printf("\n");
        printf("╔══════════════════════════════════════════════════════════╗\n");
        printf("║              LINUX SYSTEM RESOURCE MONITOR             ║\n");
        printf("╠══════════════════════════════════════════════════════════╣\n");

        printf("║ CPU Usage       : ");
        print_bar(cpu);
        printf("       ║\n");

        printf("║ Memory Usage    : ");
        print_bar(memory);
        printf("       ║\n");

        printf("║ Disk Usage      : ");
        print_bar(disk);
        printf("       ║\n");

        printf("║ Uptime          : ");
        print_uptime();
        printf("                               ║\n");

        printf("╠══════════════════════════════════════════════════════════╣\n");

        print_processes();

        printf("╠══════════════════════════════════════════════════════════╣\n");

        time_t now = time(NULL);
        struct tm *current = localtime(&now);

        printf("║ Last Updated    : %02d:%02d:%02d                             ║\n",
               current->tm_hour,
               current->tm_min,
               current->tm_sec);

        printf("║ Refresh Rate    : Every 2 seconds                       ║\n");
        printf("║ Exit            : Press Ctrl+C                          ║\n");

        printf("╚══════════════════════════════════════════════════════════╝\n");

        fflush(stdout);

        sleep(2);
    }
}
