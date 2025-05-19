CC = gcc
CFLAGS = -Os -ffreestanding -nostdlib -I.
LDFLAGS =

OBJS = main.o lcd1602.o http_client.o jsmn.o

all: weather_lcd

weather_lcd: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o weather_lcd
