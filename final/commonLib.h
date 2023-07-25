#ifndef COMMON_LIB_H
#define COMMON_LIB_H
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h> 
#define BUFFER_SIZE 36
#define SOCK_SIZE 1024
struct fileInfo {
    char name[BUFFER_SIZE];
    char path[256];
    time_t modification_time;
    off_t st_size;
    int is_directory;
    mode_t mode;
    int isDeleted;
};
void sendFileInfo(int sock,struct fileInfo *files, int countServerFile,struct fileInfo *removedFiles,int countRemovedFiles);
int receiveFileInfo(int sock,struct fileInfo *files,char *directory);
int SaveDifferentElements(struct fileInfo *files, struct fileInfo *receiveFiles, struct fileInfo *lastFiles, int countMyFile, int countRecvFile);
void writeFile(char const *fileName,int new_sock,size_t fileSize);
void readFile(char *fileName,int sockfd);
void sendFile(struct fileInfo files[256],int count,int sockfd,char *directory);
void recvFile(struct fileInfo files[256],int count,int sockfd,char *directory);
void listDir(const char *name, int indent,struct fileInfo *files, int *count,int directorySize);
void addFileData(struct fileInfo *files, int index, const char *name, const char *path, mode_t mode, off_t size, int is_directory,int directorySize);
int removeDirectory(const char *path);
int removeFileAndDirectory(const char *path);
#endif