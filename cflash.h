#ifndef CFLASH_H
#define CFLASH_H

int sanityCheck(char* file);
int isMounted(char* devPath);
int flashDevice(char* filePath, char* devPath);
int syncData();
int main(int argc, char *argv[]);

#endif //CFLASH_H
