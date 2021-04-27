//
// Created by Lukas on 26.04.2021.
//

#include "osmpexecutable.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "OSMP.h"

#include <string.h>
#include <errno.h>
extern int errno;

int main(int argc, char *argv[])/// osmpexecutable main
/// \param argc
/// \param argv
/// \return
{
    int returnVal, size, rank, source;
    //int bufin[2], bufout[2];
    int len;
    char testBuff[20] = "test123";
    char testBuff2[20] = "hallo welt";

    //sleep(1);
    pid_t pid = getpid();

    returnVal = OSMP_Init(&argc, &argv);
    if(returnVal == OSMP_ERROR)
    {
        //error handling
        printLastError(__FILE__, __LINE__);
        OSMP_Finalize();
        exit(OSMP_ERROR);
    }


    returnVal = OSMP_Size( &size );
    if(returnVal == OSMP_ERROR)
    {
        //error handling
        printLastError(__FILE__, __LINE__);
        OSMP_Finalize();
        exit(OSMP_ERROR);
    }

    returnVal = OSMP_Rank( &rank);
    if(returnVal == OSMP_ERROR)
    {
        //error handling
        printLastError(__FILE__, __LINE__);
        OSMP_Finalize();
        exit(OSMP_ERROR);
    }

    printf("child process %d - size: %d - rank: %d\n", pid, size, rank);

    if(size != 2 )
    {
        printf("too many processes running.\n");
        fflush(stdout);
        //error handling
        OSMP_Finalize();
        exit(OSMP_ERROR);
    }

    if(rank == 0)
    {
        //bufin[0] = 4711;
        //bufin[1] = 4712;
        returnVal = OSMP_Send( testBuff, 20, OSMP_BYTE, 1 );
        if(returnVal == OSMP_ERROR)
        {
            //error handling
            printLastError(__FILE__, __LINE__);
            OSMP_Finalize();
            exit(OSMP_ERROR);
        }
        printf("OSMP process %d send [%s] to %d \n", rank, testBuff, 1);
        fflush(stdout);
        returnVal = OSMP_Send( testBuff2, 20, OSMP_BYTE, 1 );
        if(returnVal == OSMP_ERROR)
        {
            //error handling
            printLastError(__FILE__, __LINE__);
            OSMP_Finalize();
            exit(OSMP_ERROR);
        }
        printf("OSMP process %d send [%s] to %d \n", rank, testBuff2, 1);
        fflush(stdout);
    }
    else if(rank == 1)
    {
        sleep(2);
        char testBuff[20] = "";
        returnVal = OSMP_Recv( testBuff, 20, OSMP_BYTE, &source, &len );
        if(returnVal == OSMP_ERROR)
        {
            //error handling
            printLastError(__FILE__, __LINE__);
            OSMP_Finalize();
            exit(OSMP_ERROR);
        }
        printf("OSMP process %d received %d byte from %d [%s] \n", rank, len, source, testBuff);

        returnVal = OSMP_Recv( testBuff, 20, OSMP_BYTE, &source, &len );
        if(returnVal == OSMP_ERROR)
        {
            //error handling
            printLastError(__FILE__, __LINE__);
            OSMP_Finalize();
            exit(OSMP_ERROR);
        }
        printf("OSMP process %d received %d byte from %d [%s] \n", rank, len, source, testBuff);
        fflush(stdout);
    }
    returnVal = OSMP_Finalize();
    exit(0);
}

void printLastError(char* file, int line) /// Prints out the last error
{
    printf("Error in file: %s - line %d - %s\n", file, line, strerror(errno));
    fflush(stdout);
}