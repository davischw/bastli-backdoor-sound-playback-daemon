CC=gcc
CFLAGS=-g
LDFLAGS=-lvlc -lrabbitmq -ljson-c


ummah.exe: libvlc_playback.o rmq_interface.o soundserver_parent_main.o main.o
	$(CC) $(CFLAGS) -lvlc -lrabbitmq -ljson-c $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
