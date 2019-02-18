# Wild macros I wrote in CS333.
The main ~~horror~~ achievement here is the variadic C macro that I wrote to iterate over multiple process lists in the xv6 kernel.
Other fun macros include a `pLock` and `pUnlock` as well as a showcase of X-Macros in a separate file.

(Note: This code attempted to stay pure ANSI C, hence the declaration-then-assignment of variables.)

`WITH_LISTS` Usage:

```c
static struct proc *
findProc(int pid)
{
  pLock;

  WITH_LISTS(ready,free,sleep,embryo,running,zombie, {
    if(item->pid == pid){
      pUnlock;
      return item;
    }
  });

  panic("Ain't got no... PID");
}
```
