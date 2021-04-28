#include <hurd/io.h>

#include "global-ports.h"
#include "util.h"
#include "mach_msg.h"

kern_return_t
io_write (io_t io, const_data_t data, mach_msg_type_number_t size,
          loff_t offset, vm_size_t *written)
{
  struct request {
    mach_msg_header_t header;
    mach_msg_type_long_t data_type;
    const char *datap;
    mach_msg_type_t offset_type;
    loff_t offset;
  };
  struct response {
    mach_msg_header_t header;
    mach_msg_type_t ret_code_type;
    kern_return_t ret_code;
    mach_msg_type_t written_type;
    vm_size_t written;
  };

  mach_msg_return_t ret;
  union {
    mach_msg_header_t header;
    struct request request;
    struct response response;
  } message;

  message.request = (struct request) {
    .header = {
      .msgh_bits = MACH_MSGH_BITS_COMPLEX |
                   MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE),
      .msgh_remote_port = io,
      .msgh_local_port = reply_port,
      .msgh_id = 21000
    },
    .data_type = {
      .msgtl_header = {
        .msgt_inline = 0,
        .msgt_longform = 1,
        .msgt_deallocate = 0
      },
      .msgtl_name = MACH_MSG_TYPE_CHAR,
      .msgtl_size = 8,
      .msgtl_number = size
    },
    .datap = data,
    .offset_type = {
      .msgt_name = MACH_MSG_TYPE_INTEGER_64,
      .msgt_size = 64,
      .msgt_number = 1,
      .msgt_inline = 1
    },
    .offset = offset
  };

  ret = mach_msg_ (&message.header, MACH_SEND_MSG|MACH_RCV_MSG,
                   sizeof message.request, sizeof message.response,
                   reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);

  if (ret)
    return ret;
  if (message.response.ret_code)
    return message.response.ret_code;

  *written = message.response.written;

  return KERN_SUCCESS;
}
