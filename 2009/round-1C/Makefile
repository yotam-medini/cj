#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2016/March/20

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu
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

${BINDIR}/allbase: obj.d/allbase.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

allbase-test: ${BINDIR}/allbase allbase-tiny.in allbase-tiny.out
	${BINDIR}/allbase allbase-tiny.in allbase-tiny.xout
	diff allbase-tiny.xout allbase-tiny.out

${BINDIR}/centermass: obj.d/centermass.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

centermass-test: ${BINDIR}/centermass centermass-tiny.in centermass-tiny.out
	${BINDIR}/centermass centermass-tiny.in centermass-tiny.xout
	diff centermass-tiny.xout centermass-tiny.out

${BINDIR}/bribe: obj.d/bribe.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

bribe-test: ${BINDIR}/bribe bribe-tiny.in bribe-tiny.out
	${BINDIR}/bribe -naive bribe-tiny.in bribe-tiny.xout
	diff bribe-tiny.xout bribe-tiny.out
	${BINDIR}/bribe bribe-tiny.in bribe-tiny.xout
	diff bribe-tiny.xout bribe-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
