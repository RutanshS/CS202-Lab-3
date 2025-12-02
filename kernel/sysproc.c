#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_hello(void) // hello syscall definition
{
  int n;
  argint(0, &n);
  print_hello(n);
  return 0;
}

// return either total running processes right now
// or total syscalls made since start
// or no. of free memory pages right now
uint64
sys_sysinfo(void) 
{
  int param;
  argint(0, &param);

  int result;
  switch(param)
  {
    case 0:
      return count_active_processes();
    case 1:
      return get_syscall_count();
    case 2:
      return count_free_pages();
    default:
      return -1;
  }
  return result;
}

struct pinfo {
  int ppid;
  int syscall_count;
  int page_usage;
};

uint64
sys_procinfo(void)
{
  uint64 user_addr;
  struct pinfo kernel_pinfo;
  struct proc *p = myproc();

  argaddr(0, &user_addr);

  // adding parent pid
  if(p->parent == 0){
    kernel_pinfo.ppid = -1;
  }else{
    kernel_pinfo.ppid = p->parent->pid;
  }

  //maintaining syscall_count excluding current procinfo call
  kernel_pinfo.syscall_count = p->syscall_counter - 1;

  //storing the page memory usage
  kernel_pinfo.page_usage = (p->sz + PGSIZE - 1) / PGSIZE;

  if(copyout(p->pagetable, user_addr, (char *)&kernel_pinfo, sizeof(kernel_pinfo)) < 0){
    return -1;
  }

  return 0; 
}

uint64
sys_sched_statistics(void)
{
  proc_stats();
  return 0;
}

uint64
sys_sched_tickets(void)
{
  int tickets;
  argint(0, &tickets);

  if (tickets >= 0 && tickets <= 10000) {
    acquire(&myproc()->lock);
    myproc()->tickets = tickets;
    myproc()->stride = 1000 / myproc()->tickets;
    release(&myproc()->lock);
  }

  return 0;
}


uint64
sys_clone(void)
{
  uint64 stack;
  argaddr(0, &stack);
  return clone((void *)stack);
}
