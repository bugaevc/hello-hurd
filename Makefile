CFLAGS += -D _GNU_SOURCE -nostdlib -static -fno-pie -I . -g

objects := $(patsubst %.c,%.o,$(wildcard *.c))
routine_objects := $(patsubst %.c,%.o,$(wildcard routines/*.c))

hello-hurd: $(objects) $(routine_objects)
	$(CC) $^ $(CFLAGS) -o $@
	objcopy --only-keep-debug $@ $@.debug
	strip --strip-debug $@

tiny: CFLAGS += -Os -flto -fvisibility=hidden -Wl,--build-id=none -Qn
tiny: CFLAGS += -fomit-frame-pointer -fno-exceptions -fno-asynchronous-unwind-tables -fno-unwind-tables
tiny: hello-hurd
	strip $^

tinier: CFLAGS += -D TINIER
tinier: tiny
	bash extra-strip.sh hello-hurd

clean:
	rm -f hello-hurd hello-hurd.debug $(objects) $(routine_objects)

.PHONY: clean
