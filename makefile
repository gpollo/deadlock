EXECS=signals                        \
      signals-safe                   \
      atfork                         \
      atfork-safe                    \
      dynamic-loader-cancel-async    \
      dynamic-loader-cancel-deferred \
      dynamic-loader-no-cancel-point \
      dynamic-loader-safe


all: clean $(EXECS)
signals: signals.c
	gcc -o $@ $? -I. -Wall -Wextra -pedantic -g -O0 -ldl -pthread

signals-safe: signals.c
	gcc -o $@ $? -I. -Wall -Wextra -pedantic -g -O0 -ldl -pthread -DSIGNAL_SAFE

atfork: atfork.c
	gcc -o $@ $? -I. -Wall -Wextra -pedantic -g -O0 -ldl -pthread

atfork-safe: atfork.c
	gcc -o $@ $? -I. -Wall -Wextra -pedantic -g -O0 -ldl -pthread -DSIGNAL_SAFE

dynamic-loader-cancel-async: dynamic-loader.c
	gcc -o $@ $? -I. -Wall -Wextra -pedantic -g -O0 -ldl -pthread -DWITH_CANCEL_ASYNC

dynamic-loader-cancel-deferred: dynamic-loader.c
	gcc -o $@ $? -I. -Wall -Wextra -pedantic -g -O0 -ldl -pthread -DWITH_CANCEL_DEFERRED

dynamic-loader-no-cancel-point: dynamic-loader.c
	gcc -o $@ $? -I. -Wall -Wextra -pedantic -g -O0 -ldl -pthread -DWITH_CANCEL_DEFERRED -DSAFE_DL_CALL

dynamic-loader-safe: dynamic-loader.c
	gcc -o $@ $? -I. -Wall -Wextra -pedantic -g -O0 -ldl -pthread -DWITH_CANCEL_DEFERRED -DSAFE_DL_CALL -DWITH_CANCEL_POINT

clean:
	rm -f $(EXECS)
