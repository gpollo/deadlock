# Signal Safety

See `signal-safety(7)` from the man pages.

## Deadlock in signal

The test program `signals` causes a deadlock when `SIGINT` is sent (e.g. with
`CTRL+C`). When the signal is sent, the main thread is blocked until the signal
handler finishes. In our case, the signal uses a mutex that might be locked by
the main thread. If the mutex is locked and the handler is executed, a deadlock
will occur.

The deadlock can be avoided while keeping the mutex by disabling the signal
the mutex is taken. The test program `signals-safe` won't deadlock when sending
`SIGINT`.

## Deadlock with fork handlers

The POSIX standard states that the function `fork(2)` is defined as
async-signal-safe. It means that it can be called within a signal handler. That
said, `signal-safety(7)` states that with POSIX.1-2003:

> if an application calls fork(2) from a signal handler and any of the fork
> handlers registered by pthread_atfork(3) calls a function that is not
> async-signal-safe, the behavior is undefined

The test program `atfork` spawns new process using `fork(2)` when `SIGINT` is
sent. It also deadlock since it uses a mutex in a its handlers set using
`pthread_atfork(3)`. The deadlock can be avoided in the same manner as the
last example. The test program `atfork-safe` won't deadlock.

