build: obj bin obj/main.o obj/dialog.o obj/myio.o obj/table.o
	gcc obj/main.o obj/dialog.o obj/myio.o obj/table.o -o bin/main

obj:
	mkdir obj

bin:
	mkdir bin

obj/main.o:
	gcc -c main.c -o obj/main.o

obj/dialog.o:
	gcc -c dialog.c -o obj/dialog.o

obj/myio.o:
	gcc -c myio.c -o obj/myio.o

obj/table.o:
	gcc -c table.c -o obj/table.o

clear:
	rm -rf obj/ bin/
