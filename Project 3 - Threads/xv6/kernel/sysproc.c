#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "spinlock.h"
#include "proc.h"
#include "sysfunc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_clone(void)
{ 
  // int clone(void(*fcn)(void*), void* arg, void* stack);
  void (*fcn)(void*);
  void *arg;
  void *stack;
  if (argptr(0, (char**)&fcn, 4) < 0) return -1;
  if (argptr(1, (char**)&arg, 4) < 0) return -1;
  if (argptr(2, (char**)&stack, 2 * PGSIZE) < 0) return -1;
  return clone(fcn, arg, stack);
}

int
sys_join(void)
{
  // int join(int pid);
  int pid;
  if (argint(0, &pid)) return -1;
  return join(pid); 
}

// BEGIN: Release the lock pointed to by lock and put the caller to sleep.  Assumes that lock is held when this is called.  When signaled, the thread awakens and reacquires the lock.
int
sys_cv_wait(void)
{
  // void cv_wait(cond_t* conditionVariable, lock_t* lock);
  cond_t* conditionVariable;
  lock_t* lock;
  if (argptr(0, (char**)&conditionVariable, 4) < 0) return -1;
  if (argptr(1, (char**)&lock, 4) < 0) return -1;
  cv_wait(conditionVariable, lock);
  return 0;
}
// END: Release the lock pointed to by lock and put the caller to sleep.  Assumes that lock is held when this is called.  When signaled, the thread awakens and reacquires the lock.

// BEGIN: Wake the threads that are waiting on conditionVariable.
int
sys_cv_signal(void)
{
  // void cv_signal(cond_t* conditionVariable);
  cond_t* conditionVariable;
  if (argptr(0, (char**)&conditionVariable, 4) < 0) return -1;
  cv_signal(conditionVariable);
  return 0;
}
// END: Wake the threads that are waiting on conditionVariable.

int
sys_find_ustack(void)
{
  // int find_ustack(int pid);
  int pid;
  if (argint(0, &pid) < 0) return -1;
  return find_ustack(pid);
}