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

## Deadlock with pthread cancel and dynamic loader

The glibc dynamic loader uses an internal mutex to protect its table of loaded
symbols. Thus, a thread calling functions such as `dlopen(3)` or `dlclose(3)`
may deadlock if it is killed during the possession of the dynamic loader lock.
It is important to add that most (if not all) call to the dynamic loader are
thread-safe, but aren't signal-safe. From `pthread_cancel(3)`:

> On  Linux, cancellation is implemented using signals.

Thus making the use of pthread cancel and async-signal-unsafe functions in the
thread being cancelled undefined behaviors. The test programs continuously
spawn, cancel and join a thread that makes calls to the dynamic loader.

The test program `dynamic-loader-cancel-async` will deadlock because the
cancelability of the thread is set the `PTHREAD_CANCEL_ASYNCHRONOUS`. It can
be cancel at any time, even inside `dlopen(3)` or `dlclose(3)` call which may
left the dynamic loader lock taken.

The test program `dynamic-loader-cancel-deferred` will deadlock. The
cancelability of the thread is set to `PTHREAD_CANCEL_DEFERRED`. When the
thread receives the cancel request, it will terminate upon the next
cancellation point (see `pthreads(7)`). While `dlopen(3)` or `dlclose(3)`
aren't cancellation points under POSIX, they may well call such functions in
their implementation with the dynamic loader lock taken.

The test program `dynamic-loader-no-cancel-point` will prevent the calls to
`dlopen(3)` and `dlclose(3)` to be cancelled by setting the cancel state
(see `pthread_setcancelstate(3)`) to `PTHREAD_CANCEL_DISABLE`. That said, the
thread won't terminate because there is no other cancellation point in the
thread routine, causing a deadlock on the `pthread_join(3)` call.

The test program `dynamic-loader-safe` disable the cancelability state too for
the dynamic loader calls, but it also adds a cancellation point with `sleep(1)`.
This program won't deadlock.
