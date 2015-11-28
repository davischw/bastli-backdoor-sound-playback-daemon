
cc = gcc
comp = ${cc} -c
link = ${cc} -lSDL_mixer -lSDL -lrabbitmq -ljson-c 


codefiles = main.c ${codefiles_with_headers}
codefiles_ummah = codefiles lookup3.c
codefiles_with_headers = soundserver_parent_main.c soundserver_child_main.c rmq_interface.c play_sound.c
headers = ${codefiles_with_headers:.c=.h}
objects = ${codefiles:.c=.o}
objects_ummah = ${codefiles_ummah:.c=.o}


all: ${objects}
	${link} -o sound_test ${objects}

main.o: main.c
	${comp} main.c

soundserver_parent_main.o: soundserver_parent_main.c soundserver_parent_main.h
	${comp} soundserver_parent_main.c

soundserver_child_main.o: soundserver_child_main.c soundserver_child_main.h
	${comp} soundserver_child_main.c

rmq_interface.o: rmq_interface.c rmq_interface.h
	${comp} rmq_interface.c

play_sound.o: play_sound.c play_sound.h
	${comp} play_sound.c

lookup3.o: lookup3.c
	${comp} lookup3.c



ummah: ${objects_ummah}
	${link} -D UMMAH -o ummah.exe ${objects_ummah}

clean:
	rm *.o
