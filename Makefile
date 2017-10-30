CC=arm-linux-gnueabihf-gcc
CFLAGS=-mthumb -march=armv7-a
CFLAGS += -g -Wall
APP=reaktionszeit
DEBUG=arm-linux-gnueabihf-gdb
DEBUGFLAGS=-tui
all:$(APP) 

$(APP):$(APP).c
	$(CC) $(CFLAGS) -o $(APP) $(APP).c
.PHONY:clean
clean:
	rm -f $(APP)
install:
	scp $(APP) root@192.168.254.254:/usr/bin
debug:
	$(DEBUG) $(DEBUGFLAGS) $(APP)
