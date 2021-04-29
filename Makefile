CFLAGS += -D _GNU_SOURCE -nostdlib -static -fno-pie -I .

objects := $(patsubst %.c,%.o,$(wildcard *.c))
routine_objects := $(patsubst %.c,%.o,$(wildcard routines/*.c))

hello-hurd: $(objects) $(routine_objects)
	$(CC) $^ $(CFLAGS) -o $@

tiny: CFLAGS += -Os -flto -fvisibility=hidden -Wl,--build-id=none -fomit-frame-pointer -fno-exceptions -fno-asynchronous-unwind-tables -fno-unwind-tables -Qn
tiny: hello-hurd
	strip $^

clean:
	rm -f hello-hurd $(objects) $(routine_objects)

.PHONY: clean
