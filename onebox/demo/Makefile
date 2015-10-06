CC=gcc
CFLAGS=-I./ -Wall -O -g 
LDFLAGS=-lyaml -lcrypt
DEBUG=

TARGET=onebox
OBJS=onebox.o util-daemon.o util-error.o util-enum.o util-pidfile.o util-cpu.o util-mem.o util-unittest.o util-debug.o util-config.o \
	util-conf-node.o util-strlcatu.o util-strlcpyu.o util-path.o test-config.o util-atomic.o util-threads.o util-pool.o \
	cli/util-cli.o cli/cli.o 

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJS):%.o:%.c
	$(CC) -g -c -fPIC $< -o $@ $(CFLAGS)  $(DEBUG)

clean:
	-rm -rf *.o $(TARGET) *~ cli/*~ cli/*.o

install:
	echo "install"
