
all: soundserver_test_main.o rmq_interface.o
	gcc -lSDL_mixer -lSDL -lrabbitmq -ljson-c -o sound_test soundserver_test_main.o rmq_interface.o

soundserver_test_main.o: soundserver_test_main.c
	gcc -c soundserver_test_main.c

rmq_interface.o: rmq_interface.c rmq_interface.h
	gcc -c rmq_interface.c

clean:
	rm *.o
