#include "part2.h"
int myDup(int oldfd)
{
    int newfd = fcntl(oldfd, F_DUPFD, 0);
    if (newfd == -1)
    {
        errno = EBADF;
        return -1;
    }
    return newfd;
}

int myDup2(int oldfd, int newfd)
{
    if (oldfd == newfd)
    {
        if (fcntl(oldfd, F_GETFL) == -1)
        {

            errno = EBADF;
            return -1;
        }
        return newfd;
    }

    if (fcntl(newfd, F_GETFD) != -1)
    {
        if (close(newfd) == -1){
            return -1;
        }
    }

    int result = fcntl(oldfd, F_DUPFD, newfd);
    if (result == -1)
    {
        errno = EBADF;
        return -1;
    }
    return result;
}