#include <mach/mach.h>

#include "global-ports.h"
#include "util.h"
#include "mach_msg.h"

kern_return_t
vm_allocate (task_t task, vm_address_t *address,
             vm_size_t size, boolean_t anywhere)
{
  struct request {
    mach_msg_header_t header;
    mach_msg_type_t address_type;
    vm_address_t address;
    mach_msg_type_t size_type;
    vm_size_t size;
    mach_msg_type_t anywhere_type;
    boolean_t anywhere;
  };
  struct response {
    mach_msg_header_t header;
    mach_msg_type_t ret_code_type;
    kern_return_t ret_code;
    mach_msg_type_t address_type;
    vm_address_t address;
  };

  mach_msg_return_t ret;
  union {
    mach_msg_header_t header;
    struct request request;
    struct response response;
  } message;

  message.request = (struct request) {
    .header = {
      .msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE),
      .msgh_remote_port = task,
      .msgh_local_port = reply_port,
      .msgh_id = 2021
    },
    .address_type = {
      .msgt_name = MACH_MSG_TYPE_INTEGER_32,
      .msgt_size = 32,
      .msgt_number = 1,
      .msgt_inline = 1
    },
    .address = *address,
    .size_type = {
      .msgt_name = MACH_MSG_TYPE_INTEGER_32,
      .msgt_size = 32,
      .msgt_number = 1,
      .msgt_inline = 1
    },
    .size = size,
    .anywhere_type = {
      .msgt_name = MACH_MSG_TYPE_BOOLEAN,
      .msgt_size = 8,
      .msgt_number = 1,
      .msgt_inline = 1
    },
    .anywhere = anywhere
  };

  ret = mach_msg_ (&message.header, MACH_SEND_MSG|MACH_RCV_MSG,
                   sizeof message.request, sizeof message.response,
                   reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);

  if (ret)
    return ret;
  if (message.response.ret_code)
    return message.response.ret_code;

  *address = message.response.address;
  return KERN_SUCCESS;
}
