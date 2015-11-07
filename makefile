
all: main.o soundserver_parent_main.o soundserver_child_main.o rmq_interface.o play_sound.o
	gcc -lSDL_mixer -lSDL -lrabbitmq -ljson-c -o sound_test soundserver_parent_main.o soundserver_child_main.o rmq_interface.o play_sound.o

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

clean:
	rm *.o
