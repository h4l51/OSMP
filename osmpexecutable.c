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
    int bufin[2], bufout[2], len;

    returnVal = OSMP_Init(&argc, &argv);
    if(returnVal == OSMP_ERROR)
    {
        //error handling
        printLastError(__FILE__, __LINE__);
        OSMP_Finalize();
        exit(OSMP_ERROR);
    }


    returnVal = OSMP_Finalize();
    return 0;

    returnVal = OSMP_Rank( &rank);
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

    if( size != 2 )
    {
        //error handling
        OSMP_Finalize();
        exit(OSMP_ERROR);
    }

    if(rank == 0)
    {
        bufin[0] = 4711;
        bufin[1] = 4712;
        returnVal = OSMP_Send( bufin, 2, OSMP_INT, 1 );
        if(returnVal == OSMP_ERROR)
        {
            //error handling
            printLastError(__FILE__, __LINE__);
            OSMP_Finalize();
            exit(OSMP_ERROR);
        }
    }
    else if(rank == 1)
    {
        returnVal = OSMP_Recv( bufout, 2, OSMP_INT, &source, &len );
        if(returnVal == OSMP_ERROR)
        {
            //error handling
            printLastError(__FILE__, __LINE__);
            OSMP_Finalize();
            exit(OSMP_ERROR);
        }
        printf("OSMP process %d received %d byte from %d [%d:%d] \n", rank, len, source, bufout[0], bufout[1]);
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