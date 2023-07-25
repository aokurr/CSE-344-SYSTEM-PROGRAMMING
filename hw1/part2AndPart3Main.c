#include "part3.h"

int main(int argc, char **argv)
{
    printf("****TEST SUCCESS MY DUP2 ****\n");
    testSuccesMyDup2();
    
    printf("\n****TEST EQUALS MY DUP2 ****\n");
    testEqualsFdMyDup2();

    printf("\n****TEST NOT VALID NEW FD MY DUP2 ****\n");
    testNotValidNewFdMyDup2();
    
    printf("\n****TEST NOT VALID SAME TWO FD MY DUP2 ****\n");
    testNotValidSameTwoFdMyDup2();
    
    printf("\n****TEST SUCCESS MY DUP ****\n");
    testSuccesMyDup();
    
    printf("\n****TEST UNSUCCESS MY DUP ****\n");
    testUnsuccesMyDup();
    
    printf("\n****TEST SUCCESS STANDART DUP2 ****\n");
    testSuccesStandartDup2();
    
    printf("\n****TEST EQUALS STANDART DUP2 ****\n");
    testEqualsFdStandartDup2();
    
    printf("\n****TEST NOT VALID NEW FD STANDART DUP2 ****\n");
    testNotValidNewFdStandartDup2();
    
    printf("\n****TEST NOT VALID SAME TWO FD STANDART DUP2 ****\n");
    testNotValidSameTwoFdStandartDup2();
    
    printf("\n****TEST SUCCESS STANDART DUP ****\n");
    testSuccesStandartDup();
    
    printf("\n****TEST UNSUCCESS STANDART DUP ****\n");
    testUnsuccesStandartDup();

    return 0;
}