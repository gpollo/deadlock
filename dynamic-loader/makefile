CFLAGS=-Wall -Wextra -pedantic -g -O0 -ldl -pthread

EXECS=$(shell basename ${PWD})-cancel-async    \
      $(shell basename ${PWD})-cancel-deferred \
      $(shell basename ${PWD})-no-cancel-point \
      $(shell basename ${PWD})-safe

all: clean $(EXECS)

$(shell basename ${PWD})-cancel-async: main.c
	gcc -o $@ $? ${CFLAGS} -DWITH_CANCEL_ASYNC

$(shell basename ${PWD})-cancel-deferred: main.c
	gcc -o $@ $? ${CFLAGS} -DWITH_CANCEL_DEFERRED

$(shell basename ${PWD})-no-cancel-point: main.c
	gcc -o $@ $? ${CFLAGS} -DWITH_CANCEL_DEFERRED -DSAFE_DL_CALL

$(shell basename ${PWD})-safe: main.c
	gcc -o $@ $? ${CFLAGS} -DWITH_CANCEL_DEFERRED -DSAFE_DL_CALL -DWITH_CANCEL_POINT

clean:
	rm -f $(EXECS)

