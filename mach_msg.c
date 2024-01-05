#include <mach/mach.h>

#include "mach_msg.h"

mach_msg_return_t
mach_msg_ (mach_msg_header_t *msg,
           mach_msg_option_t option,
           mach_msg_size_t send_size,
           mach_msg_size_t rcv_size,
           mach_port_t rcv_name,
           mach_msg_timeout_t timeout,
           mach_port_t notify)
{
#if defined (__x86_64__)
  __asm__ volatile ("syscall" "\n\t"
                    "ret"
                    :
                    : "a" (-25)
                    : "memory");
#elif defined (__i386__)
  __asm__ volatile ("lcall $0x7, $0" "\n\t"
                    "ret"
                    :
                    : "a" (-25)
                    : "memory");
#elif defined (__aarch64__)
  register int trap_x8 asm ("w8") = -25;
  __asm__ volatile ("svc #0" "\n\t"
                    "ret"
                    :
                    : "r" (trap_x8)
                    : "memory");
#else
#error Don't know how to make syscalls on this architecture
#endif
}
