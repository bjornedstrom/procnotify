all:
	gcc -Wall -Isrc -lpthread -c src/pidreader.c src/procnotify.c example.c
	gcc -lpthread procnotify.o pidreader.o example.o -o example
