#include <hurd/process.h>

#include "global-ports.h"
#include "util.h"
#include "mach_msg.h"

kern_return_t
proc_mark_exit (process_t proc, int status, int sigcode)
{
  struct request {
    mach_msg_header_t header;
    mach_msg_type_t status_type;
    int status;
    mach_msg_type_t sigcode_type;
    int sigcode;
  };
  struct response {
    mach_msg_header_t header;
    mach_msg_type_t ret_code_type;
    kern_return_t ret_code;
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
      .msgh_remote_port = proc,
      .msgh_local_port = reply_port,
      .msgh_id = 24025
    },
    .status_type = {
      .msgt_name = MACH_MSG_TYPE_INTEGER_32,
      .msgt_size = 32,
      .msgt_number = 1,
      .msgt_inline = 1
    },
    .status = status,
    .sigcode_type = {
      .msgt_name = MACH_MSG_TYPE_INTEGER_32,
      .msgt_size = 32,
      .msgt_number = 1,
      .msgt_inline = 1
    },
    .sigcode = sigcode
  };

  ret = mach_msg_ (&message.header, MACH_SEND_MSG|MACH_RCV_MSG,
                   sizeof message.request, sizeof message.response,
                   reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);

#ifndef TINIER
  if (ret)
    return ret;
  if (message.response.ret_code)
    return message.response.ret_code;
#endif

  return KERN_SUCCESS;
}
