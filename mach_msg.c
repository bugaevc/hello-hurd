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
  __asm__ volatile ("movl $-25, %%eax" "\n\t"
                    "lcall $0x7, $0" "\n\t"
                    "ret"
                    :
                    : "m" (msg), "m" (option),
                      "m" (send_size), "m" (rcv_size),
                      "m" (rcv_name), "m" (timeout),
                      "m" (notify)
                    : "eax", "memory");
}
