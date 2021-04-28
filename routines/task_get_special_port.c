#include <mach/mach.h>

#include "global-ports.h"
#include "util.h"
#include "mach_msg.h"

kern_return_t
task_get_special_port (task_t task, int which_port, mach_port_t *port)
{
  struct request {
    mach_msg_header_t header;
    mach_msg_type_t which_port_type;
    int which_port;
  };
  struct response {
    mach_msg_header_t header;
    mach_msg_type_t ret_code_type;
    kern_return_t ret_code;
    mach_msg_type_t port_type;
    mach_port_t port;
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
      .msgh_id = 2058
    },
    .which_port_type = {
      .msgt_name = MACH_MSG_TYPE_INTEGER_32,
      .msgt_size = 32,
      .msgt_number = 1,
      .msgt_inline = 1
    },
    .which_port = which_port
  };

  ret = mach_msg_ (&message.header, MACH_SEND_MSG|MACH_RCV_MSG,
                   sizeof message.request, sizeof message.response,
                   reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);

  if (ret)
    return ret;
  if (message.response.ret_code)
    return message.response.ret_code;

  *port = message.response.port;

  return KERN_SUCCESS;
}
