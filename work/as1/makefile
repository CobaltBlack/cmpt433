OUTFILE = hello
OUTDIR = $(HOME)/cmpt433/public/myApps

CC_C = arm-linux-gnueabihf-gcc 
CFLAGS = -g -std=c99 -D _POSIX_C_SOURCE=200809L -Wall -Werror

SRCS = $(wildcard *.c)

all: app

app:
	$(CC_C) $(CFLAGS) $(SRCS) -o $(OUTDIR)/$(OUTFILE)

clean:
	rm $(OUTDIR)/$(OUTFILE)
