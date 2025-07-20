#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void digestFile (const unsigned char *message, size_t messageLen, unsigned char **digest, unsigned int *digestLen) {
    EVP_MD_CTX *mdctx;

    // Hash file content using sha256
    if((mdctx = EVP_MD_CTX_new()) == NULL) perror("EVP_MD_CTX_new");
    if(EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) perror("EVP_DigestInit_ex");
    if(EVP_DigestUpdate(mdctx, message, messageLen) != 1) perror("EVP_DigestUpdate");
    if((*digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha256()))) == NULL) perror("OPENSSL_malloc");
    if(EVP_DigestFinal_ex(mdctx, *digest, digestLen) != 1) perror("EVP_DigestFinal_ex");

    EVP_MD_CTX_free(mdctx);
}

// Convert binary sha256 hash to hexadecimal
void convertDigest (unsigned const char* digest, char* result) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(result + (i * 2), "%02x", digest[i]);
    }

    // Add null byte
    result[64] = 0;
}

int readFile (char* path, char* result) {
    struct stat fileStat;
    if (stat(path, &fileStat) == -1) {
        perror("Error stating: file");
        return -1;
    }

    unsigned char *digest = NULL;
    unsigned int digest_len = 0;

    int fd = open(path, O_RDONLY);
    if(fd == -1) return -1;

    int fileSize = fileStat.st_size;
    unsigned char* buffer = malloc(fileSize);
    if (!buffer) {
        perror("Error:");
        return -1;
    }

    read(fd, buffer, fileSize);

    digestFile(buffer, fileSize, &digest, &digest_len);
    convertDigest(digest, result);

    OPENSSL_free(digest);
    free(buffer);
    close(fd);

    return 0;
}

int compareDigest (char* digestResult, char* sigFile) {
    struct stat sigFileStat;
    if (stat(sigFile, &sigFileStat) == -1) {
        perror("Error stating: sigFile");
        return -1;
    }

    int fd = open(sigFile, O_RDONLY);
    if(fd == -1) return -1;

    int sigFileSize = sigFileStat.st_size;
    char* buffer = malloc(sigFileSize);
    if (!buffer) {
        perror("Error:");
        return -1;
    }

    read(fd, buffer, sigFileSize);

    // Compare sha256 hashes which are 64 bytes
    if (strncmp(digestResult, buffer, 64) != 0) {
        printf("Warning: sha256 digests differ!\n");
        return -1;
    }

    printf(buffer);
    printf("sha256 digests match!\n");

    free(buffer);
    close(fd);

    return 0;
}
