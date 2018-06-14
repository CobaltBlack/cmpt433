OUTFILE = sorter
OUTDIR = $(HOME)/cmpt433/public/myApps

CC_C = arm-linux-gnueabihf-gcc 
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror 
THREADFLAG = -pthread

# SRCS = $(wildcard *.c)
SRCS = seg14display.c udplistener.c pot.c main.c fileutils.c i2c.c sorter.c

all: app

app:
	$(CC_C) $(CFLAGS) $(THREADFLAG) $(SRCS) -o $(OUTDIR)/$(OUTFILE)
	$(CC_C) $(CFLAGS) noworky.c -o $(OUTDIR)/noworky

clean:
	rm $(OUTDIR)/$(OUTFILE)
	rm $(OUTDIR)/noworky
