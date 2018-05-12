#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>



int main () {
    FILE *fp = popen("top", "r");
    char *ln = NULL;
    size_t len = 0;

    while (getline(&ln, &len, fp) != -1)
        fputs(ln, stdout);
        free(ln);
        pclose(fp);

    return 1;
}