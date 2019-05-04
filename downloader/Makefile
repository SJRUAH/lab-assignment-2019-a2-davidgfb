.PHONY: clean

all: clean downloader

CC=gcc
CFLAGS=-g -Wall

downloader: downloader.o
	$(CC) $(CFLAGS) -o $@ $^
	clear
	#./downloader 10 S
	#./downloader 10 P
		
	

downloader.o: downloader.c
	$(CC) $(CFLAGS) -c -o $@ $<


clean:
	@rm -rf *.o downloader
	@rm -rf descarga-*
