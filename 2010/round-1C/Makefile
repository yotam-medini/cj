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
CFLAGS = ${DBGFLAGS} -Wall -Wshadow -std=c++11 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/rope: obj.d/rope.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

rope-test: ${BINDIR}/rope rope-tiny.in rope-tiny.out
	timeout 8 ${BINDIR}/rope rope-tiny.in rope-tiny.xout
	diff rope-tiny.xout rope-tiny.out

${BINDIR}/loadtest: obj.d/loadtest.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

loadtest-test: ${BINDIR}/loadtest loadtest-tiny.in loadtest-tiny.out
	timeout 8 ${BINDIR}/loadtest loadtest-tiny.in loadtest-tiny.xout
	diff loadtest-tiny.xout loadtest-tiny.out

${BINDIR}/chessbrd: obj.d/chessbrd.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

chessbrd-test: ${BINDIR}/chessbrd chessbrd-tiny.in chessbrd-tiny.out
	timeout 8 ${BINDIR}/chessbrd chessbrd-tiny.in chessbrd-tiny.xout
	diff chessbrd-tiny.xout chessbrd-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
