//
// Created by Felix Halstrup on 08.04.21.
//

#include <stdio.h>
#include "OSMP.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


//for posix shared memory
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

char* sharedMemoryName;
int OSMP_ShmFileDescriptor;

int OSMP_Init(int *argc, char ***argv)
{
    sharedMemoryName = OSMP_GetShmName(getppid());

    OSMP_ShmFileDescriptor = shm_open(sharedMemoryName, O_RDWR, ACCESSPERMS);
    if(OSMP_ShmFileDescriptor == OSMP_ERROR)
    {
        return OSMP_ERROR;
    }
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
    switch(datatype)
    {
        case OSMP_SHORT: // short int
        {
            break;
        }
        case OSMP_INT: // int
        {
            break;
        }
        case OSMP_LONG: // long int
        {
            break;
        }
        case OSMP_UNSIGNED_CHAR: // unsinged char
        {
            break;
        }
        case OSMP_UNSIGNED: // unsigned int
        {
            break;
        }
        case OSMP_UNSIGNED_LONG: // unsigned long int
        {
            break;
        }
        case OSMP_FLOAT: // float
        {
            break;
        }
        case OSMP_DOUBLE: // double
        {
            break;
        }
        case OSMP_BYTE: // char
        {
            break;
        }
        default:
            break;
    }
    return OSMP_SUCCESS;
}
int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len)
{
    switch(datatype)
    {
        case OSMP_SHORT: // short int
        {
            break;
        }
        case OSMP_INT: // int
        {
            break;
        }
        case OSMP_LONG: // long int
        {
            break;
        }
        case OSMP_UNSIGNED_CHAR: // unsinged char
        {
            break;
        }
        case OSMP_UNSIGNED: // unsigned int
        {
            break;
        }
        case OSMP_UNSIGNED_LONG: // unsigned long int
        {
            break;
        }
        case OSMP_FLOAT: // float
        {
            break;
        }
        case OSMP_DOUBLE: // double
        {
            break;
        }
        case OSMP_BYTE: // char
        {
            break;
        }
        default:
            break;
    }
    return OSMP_SUCCESS;
}
int OSMP_Finalize(void)
{
    int returnValue;
    returnValue = shm_unlink(sharedMemoryName);
    free(sharedMemoryName);
    return returnValue;
}

char* OSMP_GetShmName(pid_t pid)
{
    char szProcessID[32];
    if(sprintf(szProcessID, "%d", pid) <= 0)
    {
        return NULL;
    }
    sharedMemoryName = calloc(strlen(OSMP_SHMEM_NAME) + strlen(szProcessID) + 1, (sizeof(char)));

    if(sprintf(sharedMemoryName, "%s%s", OSMP_SHMEM_NAME, szProcessID) <= 0)
    {
        free(sharedMemoryName);
        return NULL;
    }

    return sharedMemoryName;
}