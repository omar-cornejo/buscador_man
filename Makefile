all: buscador

buscador: buscador.c
	gcc -o buscador buscador.c
clean:
	rm -f buscador