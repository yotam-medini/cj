#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2016/March/20

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

${BINDIR}/tongues: obj.d/tongues.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

tongues-test: ${BINDIR}/tongues tongues-tiny.in tongues-tiny.out
	${BINDIR}/tongues tongues-tiny.in tongues-tiny.xout
	diff tongues-tiny.xout tongues-tiny.out

${BINDIR}/xx: obj.d/xx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

xx-test: ${BINDIR}/xx xx-tiny.in xx-tiny.out
	${BINDIR}/xx xx-tiny.in xx-tiny.xout
	diff xx-tiny.xout xx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}


