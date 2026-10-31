#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "monitor.h"

void filesystem_demo() {

    int fd;
    char buffer[100];

    printf("\n===== FILE SYSTEM =====\n");

    fd = open("monitor_test.txt",
              O_CREAT | O_RDWR,
              0644);

    if (fd < 0) {
        perror("open");
        return;
    }

    write(fd,
          "Linux File I/O Demo\n",
          20);

    lseek(fd, 0, SEEK_SET);

    int n = read(fd,
                 buffer,
                 sizeof(buffer) - 1);

    if (n > 0) {

        buffer[n] = '\0';

        printf("File Content: %s",
               buffer);
    }

    close(fd);

    struct stat file_info;

    if (stat("monitor_test.txt",
             &file_info) == 0) {

        printf("\nFile Size: %ld bytes\n",
               file_info.st_size);

        printf("Inode Number: %ld\n",
               file_info.st_ino);

        printf("File Links: %ld\n",
               file_info.st_nlink);
    }
}
