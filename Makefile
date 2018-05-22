#Para escrever comentários ##
############################# Makefile ##########################
all: teste
teste: main.o 
        # O compilador faz a ligação entre os dois objetos
		gcc -o teste main.o
#-----> Distancia com o botão TAB ### e não com espaços
main.o: main.c 
		gcc -o main.o -c main.c -W -Wall
clean:
		rm -rf *.o
		rm -rf Out*
		rm -rf Res*
mrproper: clean
		rm -rf teste
