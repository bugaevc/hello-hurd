CFLAGS = -D _GNU_SOURCE -nostdlib -static -fno-pie -g -I. -Os

objects := $(patsubst %.c,%.o,$(wildcard *.c))
routine_objects := $(patsubst %.c,%.o,$(wildcard routines/*.c))

hello-hurd: $(objects) $(routine_objects)
	$(CC) $^ $(CFLAGS) -o $@

clean:
	rm -f hello-hurd $(objects) $(routine_objects)

.PHONY: clean
