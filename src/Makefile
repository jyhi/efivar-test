DIRS = $(notdir $(shell find -type d | tail -n +2 | xargs))
OBJS = $(patsubst %,%.o,$(DIRS))
VPATH = $(DIRS)

.PHONY: all clean

all: libast.a

libast.a:
	@for i in $(DIRS); do $(MAKE) -C $$i ; done
	${AR} ${ARFLAGS} $@ ${OBJS}

clean:
	rm -f libast.a
	@for i in $(DIRS); do $(MAKE) -C $$i clean; done
