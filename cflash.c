#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int sanityCheck(int argLen, char* filePath, char* devPath) {
    struct stat fileStat;

    if (argLen != 3) {
        printf("Error: invalid number of arguments\n");
        printf("Usage: cflash [source] [destination]\n");
        return -1;
    }

    // Check file path validity
    if (stat(filePath, &fileStat) == -1) {
        perror("Bad file path");
        return -1;
    }

    // Check device path validity
    if (stat(devPath, &fileStat) == -1) {
        perror("Bad device path");
        return -1;
    }

    return 0;
}

// Abort mission if device is mounted
int isMounted(char* devPath) {
    struct stat mtabStat;

    int fd = open("/etc/mtab", O_RDONLY);
    if (fd == -1) {
        perror("Error:");
        return -1;
    }

    int fileSize = mtabStat.st_size;
    char* buffer = (char*)malloc(fileSize);

    read(fd, buffer, fileSize);

    if (strstr(buffer, devPath) != NULL) {
        printf("Caution: %s is a mounted filesystem\n", devPath);
        free(buffer);
        return -1;
    }

    free(buffer);
    close(fd);

    return 0;
}

int flashDevice(char* filePath, char* devPath) {
    // Fork and exec dd process
    pid_t pidDD = fork();

    if (pidDD < 0) {
        perror("Spawning dd process failed");
        exit(EXIT_FAILURE);
    } else if (pidDD == 0) {
        // Make file path parseable for the dd process
        char* inFileArg = "if=";
        char inFile[strlen(filePath) + strlen(inFileArg) + 1];
        snprintf(inFile, sizeof(inFile), "%s%s",inFileArg, filePath);

        // Make device path parseable for the dd process
        char* outFileArg = "of=";
        char outFile[strlen(devPath) + strlen(outFileArg) + 1];
        snprintf(outFile, sizeof(outFile), "%s%s",outFileArg, devPath);

        char *ddParams[] = {"dd", inFile, outFile, "bs=4M", "conv=fsync", "oflag=direct", "status=progress", NULL};
        execv("/bin/dd", ddParams);
        perror("Process failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int syncData() {
    // Fork and exec sync process
    pid_t pidSync = fork();

    if (pidSync < 0) {
        perror("Spawning sync process failed");
        exit(EXIT_FAILURE);
    } else if (pidSync == 0) {
        char *sync[] = {"sync", NULL};
        execv("/bin/sync", sync);
        perror("Process failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int argLen = argc;
    char* filePath = argv[1];
    char* devPath = argv[2];

    if (sanityCheck(argLen, filePath, devPath) != 0) {
        return -1;
    }

    if (isMounted(devPath) != 0) {
        return -1;
    }

    flashDevice(filePath, devPath);
    wait(NULL); // Wait for dd process before syncing
    syncData();

    return 0;
}
