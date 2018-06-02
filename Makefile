#Para escrever comentários ##
############################# Makefile ##########################
all: notebook
notebook: main.o 
        # O compilador faz a ligação entre os dois objetos
		gcc -o notebook main.o
#-----> Distancia com o botão TAB ### e não com espaços
main.o: main.c 
		gcc -o main.o -c main.c -W -Wall
clean:
		rm -rf *.o
		rm -rf Stderrfile.txt
mrproper: clean
		rm -rf notebook
