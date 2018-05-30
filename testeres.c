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
        sleep(10);
        return 0;
}