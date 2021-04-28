#include <stddef.h>

static inline void *
memcpy (void *dest, const void *src, size_t n)
{
  size_t i;

  for (i = 0; i < n; i++)
    ((char *) dest)[i] = ((const char *) src)[i];

  return dest;
}
