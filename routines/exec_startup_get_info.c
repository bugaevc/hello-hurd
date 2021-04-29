#include <hurd/exec_startup.h>

#include "global-ports.h"
#include "mach_msg.h"

kern_return_t
exec_startup_get_info_ (mach_port_t bootstrap_port,
                        mach_port_t **dtable,
                        mach_port_t **port_array)
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
    char *argvp;
    mach_msg_type_long_t envp_type;
    char *envpp;
    mach_msg_type_long_t dtable_type;
    mach_port_t *dtablep;
    mach_msg_type_long_t port_array_type;
    mach_port_t *port_array_p;
    mach_msg_type_long_t int_array_type;
    int *int_array_p;
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

  if (ret)
    return ret;
  if (message.response.ret_code)
    return message.response.ret_code;

  response = &message.response;
  *dtable = response->dtablep;
  *port_array = response->port_array_p;

  return KERN_SUCCESS;
}
