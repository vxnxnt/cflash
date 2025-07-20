#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "digest.h"

int sanityCheck(char* file) {
    struct stat fileStat;

    // Check file path validity
    if (stat(file, &fileStat) == -1) {
        perror("Bad file path");
        return -1;
    }

    return 0;
}

// Abort mission if device is mounted
int isMounted(char* devPath) {
    char mtab[] = "/etc/mtab";

    int fd = open(mtab, O_RDONLY);
    if (fd == -1) {
        perror("Error:");
        return -1;
    }

    // make buffer big enough using big enough number
    int fileSize = 512 * 1024;
    char* buffer = malloc(fileSize);
    if (!buffer) {
        perror("Error:");
        return -1;
    }


    read(fd, buffer, fileSize);

    if (strstr(buffer, devPath)) {
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
    char* sigFile = NULL;
    char* filePath = NULL;
    char* devPath = NULL;

    if (argc < 3) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            printf("Usage:\n    cflash [option] [source] [destination]\n\n");
            printf("Options:\n");
            printf("    -h, --help        shows list of command-line options\n");
            printf("    -c                crosscheck the sha256 signature of a file\n");
            return 0;
        }

        printf("Error: invalid arguments\n");
        printf("Try the -h option to get help\n");
        return -1;

    } else if (argc > 3) {
        if (strcmp(argv[1], "-c") == 0) {
            sigFile = argv[2];
            filePath = argv[3];
            devPath = argv[4];
        }

    } else if (argc == 3) {
        filePath = argv[1];
        devPath = argv[2];
    }

    if (sanityCheck(filePath) || sanityCheck(devPath) == -1) return -1;
    if (isMounted(devPath) == -1) return -1;

    if (sigFile) {
        char digestResult[65];
        if (readFile(filePath, digestResult) == -1) return -1;
        if (compareDigest(digestResult, sigFile) == -1) return -1;
    }

    flashDevice(filePath, devPath);
    wait(NULL); // Wait for dd process before syncing
    syncData();

    return 0;
}
