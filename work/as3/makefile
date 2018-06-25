OUTFILE = beatbox
OUTDIR = $(HOME)/cmpt433/public/myApps

CC_C = arm-linux-gnueabihf-gcc 
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror 
LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB
THREADFLAG = -pthread
SOUNDFLAG = -lasound

SRCS = $(wildcard *.c)

all: app wav node

app:
	$(CC_C) $(CFLAGS) $(SRCS) -o $(OUTDIR)/$(OUTFILE) $(LFLAGS) $(THREADFLAG) $(SOUNDFLAG)

wav:
	mkdir -p $(OUTDIR)/beatbox-wav-files/
	cp -R beatbox-wav-files/* $(OUTDIR)/beatbox-wav-files/
	
node:
	mkdir -p $(OUTDIR)/beatbox-server-copy/
	cp -R as3-server/* $(OUTDIR)/beatbox-server-copy/ 

clean:
	rm $(OUTDIR)/$(OUTFILE)
