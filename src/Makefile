DIRS = $(notdir $(shell find -type d | tail -n +2 | xargs))
OBJS = $(patsubst %.c,%.o,$(shell find -name '*.c' -type f | xargs))

.PHONY: all clean

all: libast.a

libast.a:
	@for i in $(DIRS); do $(MAKE) -C $$i ; done
	${AR} ${ARFLAGS} $@ $(OBJS)

clean:
	rm -f libast.a
	@for i in $(DIRS); do $(MAKE) -C $$i clean; done
