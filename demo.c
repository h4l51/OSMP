// Posix Thead Demo Programm

#include "demo.h"

// C function that is executed as a thread
void *myThreadFun(void *thargs)
{
    UNUSED(thargs);
    pthread_t mytid = pthread_self();
    sleep(1);
    printf("Greetings from Thread id = %lu\n", (unsigned long)mytid);
    return NULL;
}

int main(void)
{
    pthread_t tid[3];
    printf("Hello Thread!\n");

    // Let us create threads
    for (int i = 0; i < 3; i++)
        pthread_create(&tid[i], NULL, myThreadFun, NULL);

    for (int i = 0; i < 3; i++)
        pthread_join(tid[i], NULL);
    printf("After Thread\n");

    return 0;
}