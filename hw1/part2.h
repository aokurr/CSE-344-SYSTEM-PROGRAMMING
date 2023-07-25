#ifndef PART2
#define PART2
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
int myDup(int oldfd);
int myDup2(int oldfd, int newfd);

#endif