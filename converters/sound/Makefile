
CC = gcc
CFLAGS = -O2 -Wall -g
LDFLAGS =

.PHONY: all clean

all: conv_raw

clean:
	rm -f *.o *~

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

CONV_RAW_OBJS = conv_raw.o

conv_raw: $(CONV_RAW_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(CONV_RAW_OBJS)
