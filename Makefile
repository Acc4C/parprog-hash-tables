build/dialog: obj bin obj/main_dialog.o obj/dialog.o obj/myio.o obj/table.o
	gcc obj/main_dialog.o obj/dialog.o obj/myio.o obj/table.o -o bin/main

obj:
	mkdir obj

bin:
	mkdir bin

obj/main_dialog.o:
	gcc -c main_dialog.c -o obj/main_dialog.o

obj/dialog.o:
	gcc -c dialog.c -o obj/dialog.o

obj/myio.o:
	gcc -c myio.c -o obj/myio.o

obj/table.o:
	gcc -c table.c -o obj/table.o

clear:
	rm -rf obj/ bin/
