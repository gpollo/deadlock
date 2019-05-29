all: clean signals signals-safe atfork atfork-safe
 
signals: signals.c
	gcc -o $@ $? -I. -Wall -Wextra -pedantic -g -O0 -ldl -pthread

signals-safe: signals.c
	gcc -o $@ $? -I. -Wall -Wextra -pedantic -g -O0 -ldl -pthread -DSIGNAL_SAFE

atfork: atfork.c
	gcc -o $@ $? -I. -Wall -Wextra -pedantic -g -O0 -ldl -pthread

atfork-safe: atfork.c
	gcc -o $@ $? -I. -Wall -Wextra -pedantic -g -O0 -ldl -pthread -DSIGNAL_SAFE

clean:
	rm -f signals signals-safe atfork atfork-safe

