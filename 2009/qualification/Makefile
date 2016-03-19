# 

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

LIBDIR=/usr/lib/i386-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

DBGFLAGS = -g
CFLAGS = ${DBGFLAGS} -Wall -std=c++11 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/alien: obj.d/alien.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

alien-test: ${BINDIR}/alien alien-tiny.in alien-tiny.out
	${BINDIR}/alien alien-tiny.in alien-tiny.xout
	diff alien-tiny.xout alien-tiny.out

${BINDIR}/water: obj.d/water.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

water-test: ${BINDIR}/water water-tiny.in water-tiny.out
	${BINDIR}/water water-tiny.in water-tiny.xout
	diff water-tiny.xout water-tiny.out

${BINDIR}/welcome: obj.d/welcome.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

welcome-test: ${BINDIR}/welcome welcome-tiny.in welcome-tiny.out
	${BINDIR}/welcome welcome-tiny.in welcome-tiny.xout
	diff welcome-tiny.xout welcome-tiny.out

clean:
	rm -f ${OBJS}

-include ${DEPS}


