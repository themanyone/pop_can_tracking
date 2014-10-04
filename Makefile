NAME=ffm
INCLUDES=-I. -I/usr/include/ffmpeg
LDFLAGS=-lswscale -lavutil -lavformat -lavcodec -lz -lm
BUILD=$(NAME).o save.o funcs.o
CFLAGS=-Wall -g -pipe

%.o : %.c
	gcc $(CFLAGS) -c $< -o $@ $(INCLUDES)

test: $(BUILD)
	gcc $(BUILD) -o $(NAME) $(LDFLAGS)

s:
	scite Makefile *.h  *.c&

clean:
	rm -f *.o "${NAME}" *~ *.exe *.asm


#WINEDIR=${HOME}/Download/wine
#WRC=$(WINEDIR)/tools/wrc/wrc
