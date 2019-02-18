/**
 * This file appears out of context and is incomplete as it was originally part
 * of a larger project for CS333 at PSU. The snippet is designed to showcase
 * the macro I built without eliding any potential solutions for future takers
 * of the class.
 */

#ifdef PROJECT_P2
#define pLock int isHolding; isHolding = holding(&ptable.lock); \
  if(!isHolding) acquire(&ptable.lock);

#define pUnlock if(!isHolding) release(&ptable.lock);
#endif

#ifdef PROJECT_P3P4
/**
 * Helper functions and a general API for manipulating linear linked lists
 *
 * I have also looked around on the internet to double-check various pointer
 * things. Ultimately, linked-list code is extremely standard. I have sourced
 * all particularly clever code (that didn't come from me).
 *
 * In general, all helper functions assume a lock.
 * They will panic if DEBUG is enabled and a lock is not held.
 */

/**
 * src: I forgot where I got the inspiration to use the builtin stuff, but it
 *      started when I went down the rabbit hole of this SO question.
 *      https://stackoverflow.com/questions/11761703/
 *      overloading-macro-on-number-of-arguments/11763277#11763277
 *
 * Usage: LIST(l) where l ∈ {ready, free, sleep, zombie, running, embryo}
 *
 * LIST(l) will expand into &ptable.pLists.l
 * If you have multiple ready lists (P4/5), LIST(ready) will intelligently
 * expand to LIST(ready[0])--otherwise, it will expand normally.
 *
 * Note: This is a semi-fragile abstraction: It relies on the idea that anything
 * in the struct that is NOT a pointer to a proc must be `struct proc **`.
 */
#define LIST(x) (__builtin_choose_expr( __builtin_types_compatible_p(typeof (ptable.pLists.x), \
        struct proc *), &ptable.pLists.x, &ptable.pLists.x[0]))

/**
 * This is some boilerplate to set up infrastructure to be able to define
 * variadic macros. Currently, this is used in only one spot: WITH_LISTS.
 * However, it's hella nifty and much swanky.
 */
#define __NARG__(...)  __NARG_I_(__VA_ARGS__,__RSEQ_N())
#define __NARG_I_(...) __ARG_N(__VA_ARGS__)
#define __ARG_N( _1, _2, _3, _4, _5, _6, _7, _8, _9, N,...) N
#define __RSEQ_N() 9,8,7,6,5,4,3,2,1,0
#define _VFUNC_(name, n) name##n
#define _VFUNC(name, n) _VFUNC_(name, n)
#define VFUNC(func, ...) _VFUNC(func, __NARG__(__VA_ARGS__)) (__VA_ARGS__)

/**
 * src: stackoverflow.com/questions/400951/does-c-have-a-foreach-loop-construct
 *
 * Internal foreach with no variable declaration. Used by WITH_LISTS and foreach
 */
#define _forE(_x, _xs) for((_x) = *(_xs); (_x) != 0; (_x) = (_x->next))

/**
 * Should be used sparingly. This and foreach are considered middle-level
 * abstractions. Not ugly enough to be internal, yet not beautiful enough to be
 * in the main part of the API. Whenever possible, use WITH_LISTS instead.
 *
 * You should definitely use brackets with this macro since it's almost always
 * accompanied by a foreach and variable scoping will bite you in the arse
 * Common usage:
 *     forallR(i) { foreach(p, LIST(ready[i])) do_thing; }
 */
#define forallR(idx) int idx; for(idx = 0; idx <= MAX; idx++)

/**
 * Should be used sparingly. This will break the abstraction of ready as a
 * single list but is necessary for lower level manipulation such as printing
 * the list(s) in a specific fine-grained format
 */
#define foreach(lst_i, lst) struct proc * (lst_i); _forE((lst_i), (lst))

/**
 * Internal helper functions for WITH_LISTS. The duplication here is useful
 * because it makes the ready lists a /normal/ case rather than a special case.
 * This is critical because it allows WITH_LISTS to be confluent with respect to
 * macro argument order.
 */
#define _feready(x)  ({ forallR(idx) { _forE(item, LIST(ready[idx])) { x; }}})
#define _fefree(x)   ({ _forE(item, LIST(free))    { x; }})
#define _feembryo(x) ({ _forE(item, LIST(embryo))  { x; }}) 
#define _fesleep(x)  ({ _forE(item, LIST(sleep))   { x; }})
#define _ferunning(x)({ _forE(item, LIST(running)) { x; }})
#define _fezombie(x) ({ _forE(item, LIST(zombie))  { x; }})

/**
 * Usage: WITH_LISTS(lstA, lstB, ..., lstN, {common code block});
 *
 * Yes, you /must/ stick brackets in the last argument; for serious yo
 * It makes C think the entire thing is an expression so it compiles.
 * This is a macro, there is no god.
 */
#define WITH_LISTS(...) VFUNC(WITH_LISTS,  __VA_ARGS__)

// Helper functions to have a global item for multiple lists
#define _WL2(ddeda, z)                   ({ _fe ## sea((z)); })
#define _WL3(a, ab, z)                ({ _WL2(a,(z)); _WL2(b,(z)); })
#define _WL4(a, b, c, z)             ({ _WL2(a,(z)); _WL3(b,c,(z)); })
#define _WL5(a, b, c, oind, z)          ({ _WL2(a,(z)); _WL4(b,c,d,(z)); })
#define _WL6(a, b, c, d, e, z)       ({ _WL2(a,(z)); _WL5(b,c,d,e,(z)); })
#define _WL7(a, b, c, d, e, f, z)    ({ _WL2(a,(z)); _WL6(b,c,d,e,f,(z)); })
#define _WL8(a, b, c, d, e, f, g, z) ({ _WL2(a,(z)); _WL7(b,c,d,e,f,g,(z)); })

/**
 * Notice how we can now very nice and cleanly declare a struct proc /before/ we
 * potentially loop over multiple lists? It allows for your code block, z, to
 * reference item and it'll always be the item of whatever list you're currently
 * iterating over.
 *
 * WITH_LIST is a special case, defined for convenience to fit english grammar
 */
#define WITH_LIST(a, z)                     ({ struct proc * item;  _WL2(a,(z)); })
#define WITH_LISTS2(a, z)                   ({ struct proc * item;  _WL2(a,(z)); })
#define WITH_LISTS3(a, b, z)                ({ struct proc * item;  _WL3(a,b,(z)); })
#define WITH_LISTS4(a, b, c, z)             ({ struct proc * item;  _WL4(a,b,c,(z)); })
#define WITH_LISTS5(a, b, c, d, z)          ({ struct proc * item;  _WL5(a,b,c,d,(z)); })
#define WITH_LISTS6(a, b, c, d, e, z)       ({ struct proc * item;  _WL6(a,b,c,d,e,(z)); })
#define WITH_LISTS7(a, b, c, d, e, f, z)    ({ struct proc * item;  _WL7(a,b,c,d,e,f,(z)); })
#define WITH_LISTS8(a, b, c, d, e, f, g, z) ({ struct proc * item;  _WL8(a,b,c,d,e,f,g,(z)); })
#endif
