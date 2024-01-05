#include <mach/mach.h>

#include "mach-misc.h"

static inline long
mach_trap (int trap)
{
  long res;
#if defined (__x86_64__)
  __asm__ volatile ("syscall"
                    : "=a" (res)
                    : "a" (trap));
#elif defined (__i386__)
  __asm__ volatile ("lcall $0x7, $0"
                    : "=a" (res)
                    : "a" (trap));
#elif defined (__aarch64__)
  register int trap_x8 asm ("w8") = trap;
  register long res_x0 asm ("x0");
  __asm__ volatile ("svc #0"
                     : "=r" (res_x0)
                     : "r" (trap_x8));
  res = res_x0;
#else
#error Don't know how to make syscalls on this architecture
#endif
  return res;
}

mach_port_t
mach_reply_port ()
{
  return mach_trap (-26);
}

task_t
(mach_task_self) ()
{
  return mach_trap (-28);
}
