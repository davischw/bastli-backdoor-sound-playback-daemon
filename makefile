
all: main.o soundserver_parent_main.o soundserver_child_main.o rmq_interface.o play_sound.o lookup3.o
	gcc -lSDL_mixer -lSDL -lrabbitmq -ljson-c -o ummah.exe soundserver_parent_main.o soundserver_child_main.o rmq_interface.o play_sound.o lookup3.o main.o 

main.o: main.c
	gcc -c main.c

soundserver_parent_main.o: soundserver_parent_main.c soundserver_parent_main.h
	gcc -c soundserver_parent_main.c

soundserver_child_main.o: soundserver_child_main.c soundserver_child_main.h
	gcc -c soundserver_child_main.c

rmq_interface.o: rmq_interface.c rmq_interface.h
	gcc -c rmq_interface.c

play_sound.o: play_sound.c play_sound.h
	gcc -c play_sound.c

lookup3.o: lookup3.c
	gcc -c lookup3.c

clean:
	rm *.o
