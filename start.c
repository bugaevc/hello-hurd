#include <hurd/exec_startup.h>
#include <hurd/process.h>
#include <sys/wait.h>  // W_EXITCODE

#include "global-ports.h"
#include "main.h"

mach_port_t reply_port;
task_t task_self;

process_t proc;
io_t stdout_io, stderr_io;

static error_t
startup ()
{
  error_t err;

  mach_port_t bootstrap_port;

  vm_address_t user_entry;
  vm_address_t phdr_data;
  vm_size_t phdr_size;
  vm_address_t stack_base;
  vm_size_t stack_size;
  int flags;
  char *argv;
  mach_msg_type_number_t argc;
  char *envp;
  mach_msg_type_number_t envc;
  mach_port_t *dtable;
  mach_msg_type_number_t dtable_size;
  mach_port_t *port_array;
  mach_msg_type_number_t port_array_size;
  int *int_array;
  mach_msg_type_number_t int_array_size;

  reply_port = mach_reply_port ();
  task_self = (mach_task_self) ();

  err = task_get_bootstrap_port (task_self, &bootstrap_port);
  if (err)
    return err;

  err = exec_startup_get_info (bootstrap_port,
                               &user_entry,
                               &phdr_data, &phdr_size,
                               &stack_base, &stack_size,
                               &flags,
                               &argv, &argc,
                               &envp, &envc,
                               &dtable, &dtable_size,
                               &port_array, &port_array_size,
                               &int_array, &int_array_size);
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
