/**
 * This file appears out of context and is incomplete as it was originally part
 * of a larger project for CS333 at PSU. The snippet is designed to showcase
 * the macro I built without eliding any potential solutions for future takers
 * of the class.
 */

/**
 * Print a process listing to console. For debugging.
 * Runs when user types ^P on console.
 * No lock to avoid wedging a stuck machine further.
 */
void
procdump(void)
{
  static char * fields[] = {
#ifdef PROJECT_P2
    "PID", "Name", "UID", "GID", "PPID",
#ifdef PROJECT_P3P4
    "Prio",
#endif
    "Elapsed", "CPU", "State", "Size", "PCs",
#else /* !P2 && !P3P4 => P1 or Default */
    "PID", "State", "Name", "Elapsed", "PCs",
#endif
};

/**
 * These are the fields we print out. Proc is a macro function that is expanded
 * to the correct cprintf statement later on.
 * The macro assumes the existence of elapsed_ticks, and cpu_ticks;
 * I don't have a way to get those on the fly from p->NAME.
 */
#ifdef PROJECT_P3P4
#define PROC_FIELDS                                            \
  PROC(pid,         "%d\t")                                    \
  PROC(name,        "%s\t")                                    \
  PROC(uid,         "%d\t")                                    \
  PROC(gid,         "%d\t")                                    \
  PROC(parent->pid, "%d\t")                                    \
  PROC(priority,    "%d\t")                                    \
  cprintf("%d.%s\t", elapsed_ticks/1000, itos(elapsed_ticks)); \
  cprintf("%d.%s\t", cpu_ticks/1000,     itos(cpu_ticks));     \
  cprintf("%s\t", states[p->state]);                           \
  PROC(sz, "%d\t")
#elif defined(PROJECT_P2)
#define PROC_FIELDS                                            \
  PROC(pid,         "%d\t")                                    \
  PROC(name,        "%s\t")                                    \
  PROC(uid,         "%d\t")                                    \
  PROC(gid,         "%d\t")                                    \
  PROC(parent->pid, "%d\t")                                    \
  cprintf("%d.%s\t", elapsed_ticks/1000, itos(elapsed_ticks)); \
  cprintf("%d.%s\t", cpu_ticks/1000,     itos(cpu_ticks));     \
  cprintf("%s\t", states[p->state]);                           \
  PROC(sz, "%d\t")
#elif defined (PROJECT_P1)
#define PROC_FIELDS                                            \
  PROC(pid,         "%d")                                      \
  PROC(name,        "%s")                                      \
  PROC(uid,         "%d")                                      \
  PROC(gid,         "%d")                                      \
  PROC(parent->pid, "%d")                                      \
  cprintf("%d.%s\t", elapsed_ticks/1000, itos(elapsed_ticks));
#endif

  // Print header of procdump output
  cprintf("\n");
  int i;
  for(i = 0; fields[i+1]; i++) cprintf("%s\t", fields[i]);
  cprintf(" %s\n", fields[i]);

  // For each allocated process, print their information:
  struct proc *p;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED) continue;

    /**
     * Prints out all of the information for the proc fields.
     * This uses a concept called X-Macros.
     * src: https://natecraun.net/articles/struct-iteration-through-
     *      abuse-of-the-c-preprocessor.html
     **/
#define PROC(name, format) cprintf(format, p->name);
    PROC_FIELDS
#undef PROC

    cprintf("\n");
  }
}
