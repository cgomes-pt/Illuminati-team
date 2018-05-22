#define _BSD_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */ 
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#define BUFSIZE 128

int main () {
        size_t fd1 = open("Resultado0",O_RDONLY | O_NONBLOCK);
        size_t fd2 = open("Resultado1",O_RDONLY | O_NONBLOCK);
        size_t fd3 = open("Resultado2",O_RDONLY | O_NONBLOCK);
        char buffer[2000];
        int readed =read(fd1,buffer,256);
        close(fd1);
        write(1,buffer,readed);
       int readed2 =read(fd2,buffer,256);
       close(fd2);
        write(1,buffer,readed);
       int readed3 =read(fd3,buffer,256);
       close(fd3);
        write(1,buffer,readed);

        printf("readed1:%d,  read2:%d ,   read3:%d \n",readed,readed2,readed3);
        return 0;
}