#include <hurd/exec_startup.h>
#include <hurd/process.h>
#include <sys/wait.h>  // W_EXITCODE

#include "global-ports.h"
#include "main.h"
#include "routines/exec_startup_get_info.h"

mach_port_t reply_port;
task_t task_self;

process_t proc;
io_t stdout_io, stderr_io;

static error_t
startup ()
{
  error_t err;

  mach_port_t bootstrap_port;

  mach_port_t *dtable;
  mach_port_t *port_array;

  reply_port = mach_reply_port ();
  task_self = (mach_task_self) ();

  err = task_get_bootstrap_port (task_self, &bootstrap_port);
  if (err)
    return err;

  err = exec_startup_get_info_ (bootstrap_port,
                                &dtable, &port_array);
  if (err)
    return err;

  proc = port_array[INIT_PORT_PROC];
  stdout_io = dtable[1];
  stderr_io = dtable[2];

  return 0;
}

void _start () __attribute__ ((noreturn));

void
_start ()
{
  error_t err;
  int status;

  err = startup ();
  if (err)
    __builtin_trap ();

  status = main ();

  status = W_EXITCODE (status, 0);
  (void) proc_mark_exit (proc, status, 0);
  (void) task_terminate (task_self);

  __builtin_trap ();
}
