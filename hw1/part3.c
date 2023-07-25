#include "part3.h"
void testSuccesMyDup2()
{
    int fd1, fd2;
    struct stat fileStat1;
    struct stat fileStat2;
    fd1 = open("test1.txt", O_CREAT | O_RDWR, 0644);

    printf("fd1 writing test1.txt -> Gebze Technical University\n");
    write(fd1, "Gebze Technical University\0", 27);

    if ((fd2 = myDup2(fd1, 21)) == -1)
        perror("myDup2");

    printf("file descripter fd1 = %d, file descripter fd2 = %d\n", fd1, fd2);

    fstat(fd1, &fileStat1);
    fstat(fd2, &fileStat2);
    printf("fd1 inode value = %ld fd2 inode value = %ld\n"
                    , fileStat1.st_ino, fileStat2.st_ino);

    printf("offset fd1 = %ld,offset fd2 = %ld\n", 
    lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));

    lseek(fd2, 0, SEEK_SET);
    printf("reset  offset fd1 = %ld,offset fd2 = %ld\n", lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));
    char arr[27];
    read(fd2, arr, 27);
    printf("fd2 reading test1.txt -> %s\n", arr);

    close(fd1);
    close(fd2);
}
void testEqualsFdMyDup2()
{
    int fd1, fd2;
    struct stat fileStat1;
    struct stat fileStat2;

    fd1 = open("test1.txt", O_CREAT | O_RDWR, 0644);

    printf("fd1 writing test1.txt -> Gebze Technical University\n");
    write(fd1, "Gebze Technical University\0", 27);
    if ((fd2 = myDup2(fd1, fd1)) == -1)
        perror("MyDup2");
    printf("file descripter fd1 = %d, file descripter fd2 = %d\n", fd1, fd2);

    fstat(fd1, &fileStat1);
    fstat(fd2, &fileStat2);
    printf("fd1 inode value = %ld fd2 inode value = %ld\n", fileStat1.st_ino, fileStat2.st_ino);

    printf("offset fd1 = %ld,offset fd2 = %ld\n", lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));

    lseek(fd2, 0, SEEK_SET);
    printf("reset  offset fd1 = %ld,offset fd2 = %ld\n", lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));
    char arr[27];
    read(fd2, arr, 27);
    printf("fd2 reading test1.txt -> %s\n", arr);

    close(fd1);
    close(fd2);
}

void testNotValidNewFdMyDup2()
{
    int fd1, fd2;
    fd1 = open("test1.txt", O_CREAT | O_RDWR, 0644);

    
    if ((fd2 = myDup2(fd1, -5)) == -1){
        printf("new value = -5\n");
        perror("MyDup2");
    }
    printf("file descripter fd1 = %d, return value fd2 = %d\n", fd1, fd2);

    close(fd1);
}
void testNotValidSameTwoFdMyDup2()
{
    int fd1 = -5;
    int fd2;
    if ((fd2 = myDup2(fd1, fd1)) == -1)
        perror("myDup2");

    printf("file descripter fd1 = %d, return value fd2 = %d\n", fd1, fd2);
}

void testSuccesMyDup()
{
    int fd1, fd2;
    struct stat fileStat1;
    struct stat fileStat2;

    fd1 = open("test1.txt", O_CREAT | O_RDWR, 0644);

    printf("fd1 writing test1.txt -> Gebze Technical University\n");
    write(fd1, "Gebze Technical University\0", 27);

    if ((fd2 = myDup(fd1)) == -1)
        perror("myDup");

    printf("file descripter fd1 = %d, file descripter fd2 = %d\n", fd1, fd2);

    fstat(fd1, &fileStat1);
    fstat(fd2, &fileStat2);
    printf("fd1 inode value = %ld fd2 inode value = %ld\n", fileStat1.st_ino, fileStat2.st_ino);

    printf("offset fd1 = %ld,offset fd2 = %ld\n", lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));

    lseek(fd2, 0, SEEK_SET);
    printf("reset  offset fd1 = %ld,offset fd2 = %ld\n", lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));
    char arr[27];
    read(fd2, arr, 27);
    printf("fd2 reading test1.txt -> %s\n", arr);

    close(fd1);
    close(fd2);
}
void testUnsuccesMyDup()
{
    int fd1, fd2;
    fd1 = -5;
    if ((fd2 = myDup(fd1)) == -1)
        perror("myDup");

    printf("file descripter fd1 = %d, file descripter fd2 = %d\n", fd1, fd2);
}

void testSuccesStandartDup2()
{
    int fd1, fd2;
    struct stat fileStat1;
    struct stat fileStat2;
    fd1 = open("test1.txt", O_CREAT | O_RDWR, 0644);

    printf("fd1 writing test1.txt -> Gebze Technical University\n");
    write(fd1, "Gebze Technical University\0", 27);

    if ((fd2 = dup2(fd1, 21)) == -1)
        perror("myDup2");

    printf("file descripter fd1 = %d, file descripter fd2 = %d\n", fd1, fd2);
    fstat(fd1, &fileStat1);
    fstat(fd2, &fileStat2);
    printf("fd1 inode value = %ld fd2 inode value = %ld\n", fileStat1.st_ino, fileStat2.st_ino);
    printf("offset fd1 = %ld,offset fd2 = %ld\n", lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));

    lseek(fd2, 0, SEEK_SET);
    printf("reset  offset fd1 = %ld,offset fd2 = %ld\n", lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));
    char arr[27];
    read(fd2, arr, 27);
    printf("fd2 reading test1.txt -> %s\n", arr);

    close(fd1);
    close(fd2);
}

void testEqualsFdStandartDup2()
{
    int fd1, fd2;
    struct stat fileStat1;
    struct stat fileStat2;

    fd1 = open("test1.txt", O_CREAT | O_RDWR, 0644);
    if ((fd2 = dup2(fd1, fd1)) == -1)
        perror("Dup2");
    
    printf("fd1 writing test1.txt -> Gebze Technical University\n");
    write(fd1, "Gebze Technical University\0", 27);

    printf("file descripter fd1 = %d, file descripter fd2 = %d\n", fd1, fd2);

    fstat(fd1, &fileStat1);
    fstat(fd2, &fileStat2);
    printf("fd1 inode value = %ld fd2 inode value = %ld\n", fileStat1.st_ino, fileStat2.st_ino);

    printf("offset fd1 = %ld,offset fd2 = %ld\n", lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));

    lseek(fd2, 0, SEEK_SET);
    printf("reset  offset fd1 = %ld,offset fd2 = %ld\n", lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));
    char arr[27];
    read(fd2, arr, 27);
    printf("fd2 reading test1.txt -> %s\n", arr);

    close(fd1);
    close(fd2);
}

void testNotValidNewFdStandartDup2()
{
    int fd1, fd2;
    fd1 = open("test1.txt", O_CREAT | O_RDWR, 0644);
    fd2 = dup2(fd1, -5);

    if ((fd2 = dup2(fd1, -5)) == -1)
        perror("dup2");

    printf("file descripter fd1 = %d, file descripter fd2 = %d\n", fd1, fd2);

    close(fd1);
}
void testNotValidSameTwoFdStandartDup2()
{
    int fd1 = -5;
    int fd2;

    if ((fd2 = dup2(fd1, fd1)) == -1)
        perror("dup2");

    printf("file descripter fd1 = %d, file descripter fd2 = %d\n", fd1, fd2);
}

void testSuccesStandartDup()
{
    int fd1, fd2;
    struct stat fileStat1;
    struct stat fileStat2;

    fd1 = open("test1.txt", O_CREAT | O_RDWR, 0644);

    printf("fd1 writing test1.txt -> Gebze Technical University\n");
    write(fd1, "Gebze Technical University\0", 27);

    if((fd2 = dup(fd1))==-1)
        perror("dup");

    printf("file descripter fd1 = %d, file descripter fd2 = %d\n", fd1, fd2);

    fstat(fd1, &fileStat1);
    fstat(fd2, &fileStat2);
    printf("fd1 inode value = %ld fd2 inode value = %ld\n", fileStat1.st_ino, fileStat2.st_ino);

    printf("offset fd1 = %ld,offset fd2 = %ld\n", lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));

    lseek(fd2, 0, SEEK_SET);
    printf("reset  offset fd1 = %ld,offset fd2 = %ld\n", lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));
    char arr[27];
    read(fd2, arr, 27);
    printf("fd2 reading test1.txt -> %s\n", arr);

    close(fd1);
    close(fd2);
}
void testUnsuccesStandartDup()
{
    int fd1, fd2;

    fd1 = -5;

    if ((fd2 = dup(fd1)) == -1)
        perror("dup");

    printf("file descripter fd1 = %d, file descripter fd2 = %d\n", fd1, fd2);
}