//
// Created by Felix Halstrup on 08.04.21.
//

#include <stdio.h>
#include "OSMP.h"
#include <stdlib.h>
#include <string.h>


//for posix shared memory
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

int OSMP_Init(int *argc, char ***argv)
{
    return OSMP_SUCCESS;
}

int OSMP_Size(int *size)
{
    return OSMP_SUCCESS;
}
int OSMP_Rank(int *rank)
{
    return OSMP_SUCCESS;
}
int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest)
{
    return OSMP_SUCCESS;
}
int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len)
{
    return OSMP_SUCCESS;
}
int OSMP_Finalize(void)
{
    return OSMP_SUCCESS;
}