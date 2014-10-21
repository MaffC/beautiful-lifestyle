# Makefile configuration
CC	= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= -std=c99 -Wall -D_POSIX_C_SOURCE=199309L -Wno-missing-field-initializers -Wno-missing-braces $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm

DEBUG	= -g -O0 -DDEBUG
RELEASE	= -O2

PREFIX	= /home/pi

# Optional debugging
ifeq ($(dbg),1)
	CFLAGS += $(DEBUG)
else
	CFLAGS += $(RELEASE)
endif

# General makefile stuff
SRC	=	hue.c

OBJ	=	$(SRC:.c=.o)

BINS =	$(SRC:.c=)

all:	hue

hue:	hue.o
	@echo [link]
	@$(CC) -o $@ hue.o $(LDFLAGS) $(LDLIBS)
	@rm -f hue.o

.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@echo "[Clean]"
	@rm -f $(OBJ) *~ core tags $(BINS)

tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

# root ownership and suid bit is set to enable non-root execution
install:	hue
	@sudo rm -f $(PREFIX)/bin/hue
	@cp -a hue $(PREFIX)/bin/hue
	@sudo chown root:root $(PREFIX)/bin/hue
	@sudo chmod 4755 $(PREFIX)/bin/hue

depend:
	makedepend -Y $(SRC)
