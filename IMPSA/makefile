assemble: assemble.o tree.o error.o
	gcc -O2 -g assemble.o tree.o error.o -o assemble

assemble.o: assemble.c assemble.h
	gcc -O2 -g -c assemble.c

tree.o: tree.c tree.h
	gcc -O2 -g -c tree.c

error.o: error.c error.h
	gcc -O2 -g -c error.c
