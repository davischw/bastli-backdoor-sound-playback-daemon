CC=gcc
CFLAGS=-g
LDFLAGS=-lvlc -lrabbitmq -ljson-c

all: sound_test

ummah.exe: CFLAGS+= -D UMMAH
ummah.exe: lookup3.o libvlc_playback.o rmq_interface.o soundserver_parent_main.o main.o
	$(CC) $(CFLAGS) -lvlc -lrabbitmq -ljson-c $^ -o $@

sound_test: libvlc_playback.o rmq_interface.o soundserver_parent_main.o main.o
	$(CC) $(CFLAGS) -lvlc -lrabbitmq -ljson-c $^ -o $@

stille: stille.c
	$(CC) $(CFLAGS) `pkg-config --cflags --libs ao` $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o
