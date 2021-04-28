#include <mach/mach.h>

#include "mach-misc.h"

static inline long
mach_trap (int trap)
{
  long res;
  __asm__ volatile ("lcall $0x7, $0"
                   : "=a" (res)
                   : "a" (trap));
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
