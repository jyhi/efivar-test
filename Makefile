HOST = x86_64-w64-mingw32
CC   = ${HOST}-gcc
LD   = ${HOST}-gcc
AR   = ${HOST}-ar

DOXYGEN = doxygen

CFLAGS  = -g -ggdb3 -W -Wall
LDFLAGS =
ARFLAGS = rcs

export CC LD AR CFLAGS LDFLAGS ARFLAGS

.PHONY: all docs test clean

all: ast-efivar-test.exe docs test

ast-efivar-test.exe:
	cd src && $(MAKE)
	${LD} ${LDFLAGS} -o $@ src/libast.a

docs:
	${DOXYGEN}

test:
	echo Auch!

clean:
	rm -f ast-efivar-test.exe
	@for i in src; do cd $$i && $(MAKE) clean; done
	rm -rf docs

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $^
