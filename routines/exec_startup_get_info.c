#include <hurd/exec_startup.h>

#include "global-ports.h"
#include "util.h"
#include "mach_msg.h"

kern_return_t
exec_startup_get_info (mach_port_t bootstrap_port,
                       vm_address_t *user_entry,
                       vm_address_t *phdr_data, vm_size_t *phdr_size,
                       vm_address_t *stack_base, vm_size_t *stack_size,
                       int *flags,
                       char **argv, mach_msg_type_number_t *argc,
                       char **envp, mach_msg_type_number_t *envc,
                       mach_port_t **dtable, mach_msg_type_number_t *dtable_size,
                       mach_port_t **port_array, mach_msg_type_number_t *port_array_size,
                       int **int_array, mach_msg_type_number_t *int_array_size)
{
  struct request {
    mach_msg_header_t header;
  };
  struct response {
    mach_msg_header_t header;
    mach_msg_type_t ret_code_type;
    kern_return_t ret_code;
    mach_msg_type_t user_entry_type;
    vm_address_t user_entry;
    mach_msg_type_t phdr_data_type;
    vm_address_t phdr_data;
    mach_msg_type_t phdr_size_type;
    vm_size_t phdr_size;
    mach_msg_type_t stack_base_type;
    vm_address_t stack_base;
    mach_msg_type_t stack_size_type;
    vm_size_t stack_size;
    mach_msg_type_t flags_type;
    int flags;
    mach_msg_type_long_t argv_type;
    union {
#ifndef TINIER
        char argv[2048];
#endif
        char *argvp;
    };
    mach_msg_type_long_t envp_type;
    union {
#ifndef TINIER
        char envp[2048];
#endif
        char *envpp;
    };
    mach_msg_type_long_t dtable_type;
    union {
#ifndef TINIER
        mach_port_t dtable[512];
#endif
        mach_port_t *dtablep;
    };
    mach_msg_type_long_t port_array_type;
    union {
#ifndef TINIER
        mach_port_t port_array[512];
#endif
        mach_port_t *port_array_p;
    };
    mach_msg_type_long_t int_array_type;
    union {
#ifndef TINIER
        int int_array[512];
#endif
        int *int_array_p;
    };
  };

  mach_msg_return_t ret;
  union {
    mach_msg_header_t header;
    struct request request;
    struct response response;
  } message;
  const struct response *response;

  message.request = (struct request) {
    .header = {
      .msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE),
      .msgh_remote_port = bootstrap_port,
      .msgh_local_port = reply_port,
      .msgh_id = 30500
    }
  };

  ret = mach_msg_ (&message.header, MACH_SEND_MSG|MACH_RCV_MSG,
                   sizeof message.request, sizeof message.response,
                   reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);

#ifndef TINIER
  if (ret)
    return ret;
  if (message.response.ret_code)
    return message.response.ret_code;

  response = &message.response;

  *user_entry = response->user_entry;
  *phdr_data = response->phdr_data;
  *phdr_size = response->phdr_size;
  *stack_base = response->stack_base;
  *stack_size = response->stack_size;
  *flags = response->flags;

  *argc = response->argv_type.msgtl_number;
  if (response->argv_type.msgtl_header.msgt_inline)
    {
      ret = vm_allocate (task_self, (vm_address_t *) argv,
                         *argc, 1);
      if (ret)
        return ret;
      memcpy (*argv, response->argv, *argc);
      response = (const struct response *) ((const char *) response
                  - (sizeof response->argv - *argc));
    }
  else
    {
      *argv = response->argvp;
      response = (const struct response *) ((const char *) response
                  - (sizeof response->argv - sizeof response->argvp));
    }

  *envc = response->envp_type.msgtl_number;
  if (response->envp_type.msgtl_header.msgt_inline)
    {
      ret = vm_allocate (task_self, (vm_address_t *) envp,
                         *envc, 1);
      if (ret)
        return ret;
      memcpy (*envp, response->envp, *envc);
      response = (const struct response *) ((const char *) response
                  - (sizeof response->envp - *envc));
    }
  else
    {
      *envp = response->envpp;
      response = (const struct response *) ((const char *) response
                  - (sizeof response->envp - sizeof response->envpp));
    }

  *dtable_size = response->dtable_type.msgtl_number;
  if (response->dtable_type.msgtl_header.msgt_inline)
    {
      ret = vm_allocate (task_self, (vm_address_t *) dtable,
                         *dtable_size * sizeof (mach_port_t), 1);
      if (ret)
        return ret;
      memcpy (*dtable, response->dtable,
              *dtable_size * sizeof (mach_port_t));
      response = (const struct response *) ((const char *) response
                  - (sizeof response->dtable - *dtable_size * sizeof (mach_port_t)));
    }
  else
    {
      *dtable = response->dtablep;
      response = (const struct response *) ((const char *) response
                  - (sizeof response->dtable - sizeof response->dtablep));
    }

  *port_array_size = response->port_array_type.msgtl_number;
  if (response->port_array_type.msgtl_header.msgt_inline)
    {
      ret = vm_allocate (task_self, (vm_address_t *) port_array,
                         *port_array_size * sizeof (mach_port_t), 1);
      if (ret)
        return ret;
      memcpy (*port_array, response->port_array,
              *port_array_size * sizeof (mach_port_t));
      response = (const struct response *) ((const char *) response
                  - (sizeof response->port_array - *port_array_size * sizeof (mach_port_t)));
    }
  else
    {
      *port_array = response->port_array_p;
      response = (const struct response *) ((const char *) response
                  - (sizeof response->port_array - sizeof response->port_array_p));
    }

  *int_array_size = response->int_array_type.msgtl_number;
  if (response->int_array_type.msgtl_header.msgt_inline)
    {
      ret = vm_allocate (task_self, (vm_address_t *) int_array,
                         *int_array_size * sizeof (int), 1);
      if (ret)
        return ret;
      memcpy (*int_array, response->int_array,
              *int_array_size * sizeof (int));
      response = (const struct response *) ((const char *) response
                  - (sizeof response->int_array - *int_array_size * sizeof (int)));
    }
  else
    {
      *int_array = response->int_array_p;
      response = (const struct response *) ((const char *) response
                  - (sizeof response->int_array - sizeof response->int_array_p));
    }
#else
  *dtable = message.response.dtablep;
  *port_array = message.response.port_array_p;
#endif

  return KERN_SUCCESS;
}
