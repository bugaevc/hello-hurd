#include <hurd/io.h>

#include "global-ports.h"

static const char message[] = "Hello, Hurd!\n";
static const char error_message[] = "Failed to write\n";

int
main ()
{
  const char *data;
  size_t written;
  size_t written_this_time;
  error_t err;

  data = message;

  for (written = 0; written < sizeof message - 1;)
    {
      err = io_write (stdout_io, data,
                      sizeof message - 1 - written,
                      -1, &written_this_time);
#ifdef TINIER
      return 0;
#endif

      if (err)
        {
          io_write (stderr_io, error_message,
                    sizeof error_message - 1,
                    -1, &written_this_time);
          return 1;
        }
      written += written_this_time;
      data += written_this_time;
    }

  return 0;
}

