#ifndef DIGEST_H
#define DIGEST_H

void digestFile (const unsigned char *message, size_t messageLen, unsigned char **digest, unsigned int *digestLen);
void convertDigest (unsigned const char* digest, char* result);
int readFile (char* path, char* result);
int digestSigFile (char* sigFile);
int compareDigest (char* digestResult, int isFile, char* sigFile);

#endif //DIGEST_H
