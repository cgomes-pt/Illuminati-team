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
    int stdin;
}CM;
CM cmds[BUFSIZE];
int numcomandos=0;

void passaParaEstrutura(const char* dump_path) {
    int fd = open(dump_path,O_RDONLY,0666);
    char buffer[256];
    char *buf = buffer;
    int rd;
    char* tk;
    while ( 1 ) {
        rd = readln(fd,buf,258);
        if (rd<=0) break;
        buf[rd]='\0';
        if(buf[0]=='$') {
            cmds[numcomandos].numargs=0;
            cmds[numcomandos].stdin=-1;
            if(rd>1 && (buf[1]=='|' || buf[2]=='|' ) ) {
                if (buf[1]=='|'){
                    buf= buf + 3; // tira o | $ espaço
                    cmds[numcomandos].stdin=numcomandos-1;
                }
                else {
                    tk = strtok(buf, "|"); //tk = $num
                    tk = tk + 1 ; //tk =num
                    cmds[numcomandos].stdin=numcomandos-atoi(tk);
                    tk = strtok(NULL, "|"); //tk = comando
                    buf=tk;
                    buf = buf +1 ;//tira espaço
                } 
                cmds[numcomandos].compipe = 0;
            }
            else{
                if (rd>1)
                    cmds[numcomandos].compipe = 1;

                buf= buf + 2; // tira o $ e um espaço
            }
            
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

}


int correComando(int i) { // i== comando i a correr

    char resultado[11];
    char fichinput[11];
    int fdinput;
    int w;
    
    int fderro;
    if((fderro=open("Stderrfile.txt",O_CREAT|O_WRONLY,0644))!=-1){
        dup2(fderro,2);
    }
    else printf("Erro abrir Stderrfile.txt\n");
    
    sprintf( resultado, "Resultado%d",i); 
    int fdres=0;

    cmds[i].args[cmds[i].numargs]=NULL;
    if (cmds[i].compipe==0 && cmds[i].stdin>=0) { // ==0 tem de ler input do comando anterior
        if(i==0) {
            printf("ERRO-o primeiro comando não pode ter pipe");
            return -1;
        }
        sprintf( fichinput, "Resultado%d",cmds[i].stdin);
        fdinput = open(fichinput,O_RDONLY,0666); 
        
        if (fdinput<0) {
            printf("erro abrir ficheiro");
            perror("r2");
            return -1;
        }
    }

    int f=fork();

    if(f!=0)  wait(&w); //pai

    else { //filho
        if (cmds[i].compipe==0 && i!=0) dup2(fdinput,0);

            fdres = open(resultado,O_WRONLY |O_CREAT,0666);
            if (fdres<0)  {
                perror("r1");
                return -1;
            }
            dup2(fdres,1);
            execvp(cmds[i].args[0],cmds[i].args);
            exit(33); // WEXITSTATUS(w) é 33 se a execução falhar

    }

  close(fdinput);
  close(fdres);
  close(fderro);

  
  return WEXITSTATUS(w);

}

void atualizaFicheiro (char* file) {
    char buffer[1500];

    ssize_t rd; ssize_t output;
    int fdNB,fdTemp, fdres;

    fdNB = open(file,O_RDONLY,0666);
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

    fdNB = open(file,O_WRONLY,0666);
    fdTemp = open("tmp.nb",O_RDONLY, 0666);
    rd = read(fdTemp,buffer,1500);
    if (rd>=0)
            write(fdNB,buffer,rd);

    close(fdNB);
    close(fdTemp);
    remove("tmp.nb");



}


int checkStderr() {
        
        char buffer[1500];
        int fderro = open("Stderrfile.txt",O_RDONLY,0666);
        if (fderro<0) return -1;
        int rd = read(fderro,buffer,1500);
        return rd;
           
}


int main (int argc, char* argv[]) {
    int erros=0;
    int condicao = 0;
    int ret=-1;
    if (argc < 2) {
        fprintf(stderr, "Preciso do ficheiro do notebook como argumento.\n");
        exit(1);
    }
    passaParaEstrutura(argv[1]);

    int i;
    for( i=0;i<numcomandos;i++){
        condicao = correComando(i);
        erros= checkStderr();
        if (condicao==33 || erros!=0 || condicao==-1) break;
    }

    if (condicao!=33 && erros==0 && condicao!=-1) {
        atualizaFicheiro(argv[1]);
        ret=0;
    }
    else {
       if (condicao==33) printf("Erro no Comando %s.\nNotebook não alterado.\n",cmds[i].args[0]);
       if (erros!=0) printf("Comando %s escreveu para o stderr.\nNotebook não alterado.\n",cmds[i].args[0]);
      
    }
    char resultado[11];
    for( i=0;i<numcomandos;i++){
        sprintf( resultado, "Resultado%d",i); 
        remove(resultado);
    }

    return ret;
}


