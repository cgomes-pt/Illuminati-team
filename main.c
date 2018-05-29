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
 

ssize_t readln(int fildes, void *buf, size_t nbyte){
	int rd;
	size_t lidas=0;
	char *p=buf;
	while ( (rd=read(fildes,p,1))>=0 && *p!='\n' ) {
		if (nbyte<lidas) return lidas;
		if (rd==0) return lidas;
		if (rd<0) return -1;
		lidas++;
		p++;
	}	
	return lidas; 
}

typedef struct comando {
    char *args[BUFSIZE]; 
    int numargs; //numero de args;
    int compipe;  // se compipe==0 tem '$|' senão é '$'
  //  int fdpipe[2]; //2 pipes com o output deste comando  , fdpipe[0] vai ser lido para o possivel input do proximo comando, fdpipe[1] é para imprimir no notebook
}comandos;

struct comando cmds[BUFSIZE];
int numcomandos=0;
/*
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

*/


void passaParaEstrutura(const char* dump_path) {
    int fd = open(dump_path,O_RDONLY,0666);
    char buffer[256];
    char *buf = buffer;
    int rd;

    while ( 1 ) {
        rd = readln(fd,buf,258);
      //  printf("rd=%d",rd);
        if (rd<=0) break;
        buf[rd]='\0';
        if(buf[0]=='$') {
        //           printf("buf=%s\n",buf);
            //struct comando cmd;
           // cmd =malloc(sizeof(char*) + sizeof(int) + sizeof(int) );
            cmds[numcomandos].numargs=0;
            if(rd>1 && buf[1]=='|') {
                cmds[numcomandos].compipe = 0;
                buf= buf + 1; // tira o |

            }
            else{
                cmds[numcomandos].compipe = 1;
            }
            buf= buf + 2; // tira o $ e um espaço
            
            const char s[2] = " ";
            char *token;
            token = strtok(buf, s);
            while( token != NULL ) {
              //  printf( " %s\n", token );
                cmds[numcomandos].args[cmds[numcomandos].numargs] =malloc(sizeof(char*));
                strcpy(cmds[numcomandos].args[cmds[numcomandos].numargs],token);
                cmds[numcomandos].numargs++; 
                token = strtok(NULL, s);
            }
            numcomandos++;
        }
    }
    /*
    int i,j;
    for( i=0;i<numcomandos;i++){
        for ( j=0; j<cmds[i].numargs;j++) {
       //     if (j+1==cmds[i].numargs)  cmds[i].args[j+1]=NULL;
            printf("Comando%d,Argumento%d::::%s\n",i,j,cmds[i].args[j]);
        } 
    }

*/
}


int correComando(int i) { // i== comando i a correr
    
    char resultado[11];
    char fichoutput[11];
    char fichinput[11];
    size_t fdinput;
    
    sprintf( resultado, "Resultado%d",i); 
   // sprintf( fichoutput, "Output%d",i);

   // size_t fifoout = mkfifo(fichoutput,0666);
    size_t fifores=0,fdres=0;
   //  fifores = mkfifo(resultado,0666);

   // if (fifoout<0 || fifores<0) {
     //   printf("error mkfifo\n"); return -1;
   // }

  //  size_t fd = open(fichoutput,O_WRONLY| O_NONBLOCK );
    
  //  if (fd<0)  {
    //    perror("r1");
      //  return -1;
   // }

    cmds[i].args[cmds[i].numargs]=NULL;
    if (cmds[i].compipe==0) { // ==0 tem de ler input do comando anterior
        if(i==0) {
            printf("ERROR-Cant put a pipe on the first command");
            return -1;
        }
        sprintf( fichinput, "Resultado%d",i-1);
        fdinput = open(fichinput,O_RDONLY,0666); 
        
        if (fdinput<0) {
            printf("erro abrir pipe");
            perror("r2");
            return -1;
        }
    }

    int f=fork();

    if(f!=0)  wait(NULL); //pai0

    else { //filho0
        if (cmds[i].compipe==0 && i!=0) dup2(fdinput,0);
     //   int f2=fork();
       int f2=0;
        if(f2==0) { //filho1
            fdres = open(resultado,O_WRONLY |O_CREAT,0666);
            if (fdres<0)  {
                perror("r1");
                return -1;
            }
            dup2(fdres,1);
            execvp(cmds[i].args[0],cmds[i].args);
            exit(-1);
        }
    //    dup2(fd,1);
      //  wait(NULL);
       // execvp(cmds[i].args[0],cmds[i].args);
       // exit(-1);
    }

 // close(fd);
  close(fdinput);
  close(fdres);
  return 0;

}

void atualizaFicheiro () {
    char buffer[1500];

    ssize_t rd; ssize_t output;
    int fdNB,fdTemp, fdres;

    fdNB = open("exemplo.nb",O_RDONLY,0666);
    fdTemp = open("tmp.nb",O_WRONLY |O_APPEND |  O_CREAT, 0666);

    if(fdNB==-1 || fdTemp==-1 ){
        printf("file not found.\n");
        return;
    }

    char resultado[11];
    int i=0;
    int oldres=0;
    while (1) {
        
        rd = readln(fdNB,buffer,1500);
        if (rd<=0) break;
        
        if ( oldres==0 && buffer[0]=='$') {
            
            write(fdTemp,buffer,rd);
            sprintf( resultado, "Resultado%d",i);
            fdres = open(resultado,O_RDONLY,0666);
            if(fdres==-1){
                printf("file not found.\n");
                return;
            }

            while (output=read(fdres,buffer,1500)) {
                write(fdTemp,"\n>>>\n",5);
                write(fdTemp,buffer,output);
                write(fdTemp,"<<<\n",4);
            }
            i++;
            close(fdres);
        }
        else {
            if (buffer[0]=='>'&&buffer[1]=='>'&&buffer[2]=='>') {
                oldres=1;
            }
            if (oldres==0) {
                write(fdTemp,buffer,rd);
                write(fdTemp,"\n",1);
            }
            if (buffer[0]=='<'&&buffer[1]=='<'&&buffer[2]=='<') {
                oldres=0;
            }

        }

    }

    close(fdNB);
    close(fdTemp);

    fdNB = open("exemplo.nb",O_WRONLY,0666);
    fdTemp = open("tmp.nb",O_RDONLY, 0666);
    rd = read(fdTemp,buffer,1500);
    if (rd>=0)
            write(fdNB,buffer,rd);

    close(fdNB);
    close(fdTemp);
    remove("tmp.nb");



}





int main (int argc, char* argv[]) {
    
    if (argc < 2) {
        fprintf(stderr, "Preciso do ficheiro.\n");
        exit(1);
    }
  // printf("\033[H\033[J");
    passaParaEstrutura(argv[1]);

    int i;
    for( i=0;i<numcomandos;i++){
        correComando(i);
    }

    atualizaFicheiro();
    return 0;
}


