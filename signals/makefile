CFLAGS=-I../common/ -Wall -Wextra -pedantic -g -O0 -ldl -pthread

EXECS=$(shell basename ${PWD}) \
      $(shell basename ${PWD})-safe

all: clean $(EXECS)

$(shell basename ${PWD}): main.c
	gcc -o $@ $? ${CFLAGS}

$(shell basename ${PWD})-safe: main.c
	gcc -o $@ $? ${CFLAGS} -DSIGNAL_SAFE

clean:
	rm -f $(EXECS)

