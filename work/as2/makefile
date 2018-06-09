OUTFILE = sorter
OUTDIR = $(HOME)/cmpt433/public/myApps

#CC_C = gcc 
CC_C = arm-linux-gnueabihf-gcc 
CFLAGS = -g -std=c99 -D _POSIX_C_SOURCE=200809L -Wall -Werror -pthread

SRCS = $(wildcard *.c)

all: app

app:
	$(CC_C) $(CFLAGS) $(SRCS) -o $(OUTDIR)/$(OUTFILE)

clean:
	rm $(OUTDIR)/$(OUTFILE)
