#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 128
 
typedef struct auxiliar {
    char *resultado;
    struct auxiliar *next;
     
};
struct auxiliar outputs[BUFSIZE];

typedef struct file {
    char *conteudo;
}*ficheiro;






int parse_output(char *cmd) {
    
    int i = 0;
    
    char buf[BUFSIZE];
    FILE *fp;

    if ((fp = popen(cmd, "r")) == NULL) {
        printf("Error opening pipe!\n");
        return -1;
    }

    while (fgets(buf, BUFSIZE, fp) != NULL) {
        sscanf(buf,"%s", &outputs[i].resultado);
        printf("OUTPUT 1 : %s\n", &(outputs[i].resultado));i++;

    }
    

    if(pclose(fp))  {
        printf("Command not found or exited with error status\n");
        return -1;
    }

    return 0;
}



char* apagarDollarIcone (char *buffer) {
    int i = 0 ;
    int j = 0;
    char *resultado=malloc(sizeof(char));
   
   
    for (i=0;buffer[i]!='\0';i++) {

        if (buffer[i]!='$') {
            resultado[j++] = buffer[i];

        }


    }
    return resultado;
}

void lerFicheiro (const char* dump_path) {

    int c;
    FILE *file;
    file = fopen(dump_path, "r");
    char buffer[256];
    char *teste;

    
    while(fgets(buffer, 256, file)) {
         if (buffer[0]=='$' && buffer[1] !='|') {
             printf ("%s",apagarDollarIcone("$ ls"));
             printf ("%d\n",parse_output(apagarDollarIcone("$ ls")));
         }
    
    }
    fclose(file);
     
}



int main () {
    lerFicheiro("exemplo.nb");
    return 1;
}