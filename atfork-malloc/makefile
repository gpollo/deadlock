CFLAGS += -I. -Wall -Wextra -pedantic -g -O0 -pthread --std=c11

EXECS=$(shell basename ${PWD}) \
      $(shell basename ${PWD})-join-parent

all: clean $(EXECS)

$(shell basename ${PWD}): main.c
	gcc -o $@ $? $(CFLAGS) -DJOIN_IN_HANDLER

$(shell basename ${PWD})-join-parent: main.c
	gcc -o $@ $? $(CFLAGS)

clean:
	rm -f $(EXECS)

