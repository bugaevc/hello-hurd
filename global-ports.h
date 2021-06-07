#include <mach/mach.h>
#include <hurd/io.h>

extern mach_port_t reply_port;

#ifndef TINIER
extern task_t task_self;
#endif

extern process_t proc;
extern io_t stdout_io, stderr_io;
