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
OSMP_shm_info* infoStruct;

int nRank = -1;

size_t OSMP_SizeTable[] =
        {
                [OSMP_SHORT] = sizeof(short int),
                [OSMP_INT] = sizeof(int),
                [OSMP_LONG] = sizeof(long int),
                [OSMP_UNSIGNED_CHAR] = sizeof(unsigned char),
                [OSMP_UNSIGNED] =  sizeof(unsigned int),
                [OSMP_UNSIGNED_LONG] = sizeof(unsigned long int),
                [OSMP_FLOAT] = sizeof(float),
                [OSMP_DOUBLE] = sizeof(double),
                [OSMP_BYTE] = sizeof(char)
        };

int OSMP_Init(int *argc, char ***argv)
{
    struct stat shmStat;
    int returnVal;

    sharedMemoryName = OSMP_GetShmName(getppid());

    OSMP_ShmFileDescriptor = shm_open(sharedMemoryName, O_RDWR, ACCESSPERMS);
    if(OSMP_ShmFileDescriptor == OSMP_ERROR)
    {
        return OSMP_ERROR;
    }
    returnVal = fstat(OSMP_ShmFileDescriptor, &shmStat);
    if(returnVal == OSMP_ERROR)
    {
        return OSMP_ERROR;
    }

    infoStruct = (OSMP_shm_info*)mmap(NULL, (size_t)shmStat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, OSMP_ShmFileDescriptor, 0);
    if(infoStruct == MAP_FAILED)
    {
        return OSMP_ERROR;
    }

    pid_t pid = getpid();
    for(int i = 0; i < infoStruct->nProcessCount; i++)
    {
        if(infoStruct->proc[i].pid == pid)
        {
            nRank = i;
            break;
        }
    }
    return OSMP_SUCCESS;
}

int OSMP_Size(int *size)
{
    if(size == NULL)
        return OSMP_ERROR;

    *size = infoStruct->nProcessCount;
    return OSMP_SUCCESS;
}
int OSMP_Rank(int *rank)
{
    if(rank == NULL || nRank == -1)
        return OSMP_ERROR;

    *rank = nRank;
    return OSMP_SUCCESS;
}
int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest)
{
    OSMP_Message* pMessageDest = &infoStruct->messages[0];
    size_t nBytesToSend = 0;

    if(enterCriticalSectionWrite() == OSMP_ERROR)
        return OSMP_ERROR;

    for(int i = 0; i < OSMP_MAX_SLOTS; i++)
    {
        if(pMessageDest->buffer[0] == '\0')
        {
            break;
        }
        else if(i == OSMP_MAX_SLOTS - 1)
        {
            leaveCriticalSectionWrite();
            return OSMP_ERROR; // no available space
        }
        pMessageDest++;
    }

    int rank;
    if(OSMP_Rank(&rank) == OSMP_ERROR)
    {
        leaveCriticalSectionWrite();
        return OSMP_ERROR;
    }

    pMessageDest->nSenderID = rank;
    pMessageDest->nReceiverID = dest;

    nBytesToSend = (size_t)count * sizeOfType(datatype);
    memcpy(pMessageDest->buffer, buf, nBytesToSend);

    leaveCriticalSectionWrite();
    return OSMP_SUCCESS;
}
int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len)
{

    int rank;
    size_t bufferSize, buffElementCount, datatypeSize;

    if(OSMP_Rank(&rank) == OSMP_ERROR)
    {
        return OSMP_ERROR;
    }

    if(enterCriticalSectionRead() == OSMP_ERROR)
        return OSMP_ERROR;

    OSMP_Message* pMessageSource = &infoStruct->messages[0];
    for(int i = 0; i < OSMP_MAX_SLOTS - 1; i++)
    {
        if(pMessageSource->nReceiverID == rank)
            break;

        if(i == OSMP_MAX_SLOTS - 1)
        {
            leaveCriticalSectionRead();
            return OSMP_ERROR; // no message for process found
        }


        pMessageSource++;
    }

    *source = pMessageSource->nSenderID;

    bufferSize = sizeof(pMessageSource->buffer);
    datatypeSize = sizeOfType(datatype);

    buffElementCount = bufferSize / (size_t)datatypeSize;
    if(buffElementCount > count)
        bufferSize = (size_t)count * datatypeSize;

    *len = (int)bufferSize;
    memcpy(buf, pMessageSource->buffer, bufferSize);

    leaveCriticalSectionRead();

    //delete message
    enterCriticalSectionWrite();
    memset(pMessageSource, 0, sizeof(OSMP_Message));
    leaveCriticalSectionWrite();

    return OSMP_SUCCESS;
}
int OSMP_Finalize(void)
{
    int returnVal;
    int out = OSMP_SUCCESS;
    struct stat shmStat;

    returnVal = fstat(OSMP_ShmFileDescriptor, &shmStat);
    if(returnVal != OSMP_ERROR)
    {
        returnVal = munmap(infoStruct, (size_t)shmStat.st_size);
        if(returnVal == OSMP_ERROR)
        {
            out = OSMP_ERROR;
        }
    }
    else
    {
        out = OSMP_ERROR;
    }

    free(sharedMemoryName);
    return out;
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

size_t sizeOfType(OSMP_Datatype type)
{
    if((type >= 0) && (type < sizeof(OSMP_SizeTable)/sizeof(size_t)))
        return OSMP_SizeTable[type];
    else
        return 0;
}

int enterCriticalSectionWrite()
{
    if(sem_wait(&infoStruct->sem_write) == OSMP_ERROR)
        return OSMP_ERROR;
    return OSMP_SUCCESS;
}

int leaveCriticalSectionWrite()
{
    if(sem_post(&infoStruct->sem_write) == OSMP_ERROR)
        return OSMP_ERROR;
    return OSMP_SUCCESS;
}

int enterCriticalSectionRead()
{
    if(sem_wait(&infoStruct->sem_readCount) == OSMP_ERROR)
    {
        return OSMP_ERROR;
    }
    infoStruct->readCount++;
    if(infoStruct->readCount == 1)
    {
        if(sem_wait(&infoStruct->sem_write) == OSMP_ERROR)
        {
            infoStruct->readCount--;
            sem_post(&infoStruct->sem_readCount);
            return OSMP_ERROR;
        }
    }
    if(sem_post(&infoStruct->sem_readCount) == OSMP_ERROR)
        return OSMP_ERROR;
    return OSMP_SUCCESS;
}

int leaveCriticalSectionRead()
{
    int returnVal = OSMP_SUCCESS;
    if(sem_wait(&infoStruct->sem_readCount) == OSMP_ERROR)
    {
        return OSMP_ERROR;
    }
    infoStruct->readCount--;
    if(infoStruct->readCount == 0)
    {
        if(sem_post(&infoStruct->sem_write) == OSMP_ERROR)
            returnVal = OSMP_ERROR;
    }
    if(sem_post(&infoStruct->sem_readCount) == OSMP_ERROR)
        returnVal = OSMP_ERROR;
    return returnVal;
}