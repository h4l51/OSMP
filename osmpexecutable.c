//
// Created by Lukas on 26.04.2021.
//

#include "osmpexecutable.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "OSMP.h"

int main(int argc, char *argv[])/// osmpexecutable main
/// \param argc
/// \param argv
/// \return
{
    int returnVal, size, rank, source;
    int bufin[2], bufout[2], len;

    if((returnVal = OSMP_Init(&argc, &argv)) == OSMP_ERROR)
    {
        //error handling
    }
    if((returnVal = OSMP_Rank( &rank )) == OSMP_ERROR)
    {
        //error handling
    }
    if((returnVal = OSMP_Size( &size )) == OSMP_ERROR)
    {
        //error handling
    }

    if( size != 2 )
    {
        //error handling
    }

    if(rank == 0)
    {
        bufin[0] = 4711;
        bufin[1] = 4712;
        if((returnVal = OSMP_Send( bufin, 2, OSMP_INT, 1 )) == OSMP_ERROR)
        {
            //error handling
        }
    }
    else if(rank == 1)
    {
       if((returnVal = OSMP_Recv( bufout, 2, OSMP_INT, &source, &len )) == OSMP_ERROR)
       {
           //error handling
       }
       printf("OSMP process %d received %d byte from %d [%d:%d] \n", rank, len, source, bufout[0], bufout[1]);

    }

    exit(0);
}