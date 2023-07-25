#ifndef SERVERLIB_H
#define SERVERLIB_H
#include "FifoHeader.h"
#include <dirent.h>
#include <pthread.h>

int handle_client(char *buffer, pid_t pid, int clientFd);
int logFile(pid_t pid, int clientNumber, int logFileDescriptor, int flag);
char *helpRequest(char *arr);
int readFile(const char *filename, int line, char *resp);
int writeFile(const char *filename, int lineNumber, const char *str);
#endif