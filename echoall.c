//
// Created by Lukas Böing on 08.04.21. for Praktikum 1
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *username = getenv("USER");
    char *path = getenv("PATH");
    pid_t pid = getpid();

    printf("\nECHO > pid: %d Arguments:{ ", pid);
    for(int i = 0; i < argc; i++)
    {
        printf("'%s' ", argv[i]);
        
    }
    printf("} Environment:{ user: '%s'}", username);
    //printf("} Environment:{ user: '%s' path: '%s' }", username, path);
    return 0;
}