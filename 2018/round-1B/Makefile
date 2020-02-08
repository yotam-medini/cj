#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/April/29
# codingcompetitions.withgoogle.com/codejam/round/0000000000007764

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

DBGFLAGS = -g
CFLAGS = ${DBGFLAGS} -Wall -Wshadow -std=c++14 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/rounding: obj.d/rounding.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

rounding-test: ${BINDIR}/rounding rounding-tiny.in rounding-tiny.out
	timeout 8 ${BINDIR}/rounding -naive rounding-tiny.in rounding-tiny.xnout
	diff rounding-tiny.xnout rounding-tiny.out
	timeout 8 ${BINDIR}/rounding rounding-tiny.in rounding-tiny.xout
	diff rounding-tiny.xout rounding-tiny.out

${BINDIR}/signs: obj.d/signs.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

signs-test: ${BINDIR}/signs signs-tiny.in signs-tiny.out
	timeout 8 ${BINDIR}/signs -naive signs-tiny.in signs-tiny.xnout
	diff signs-tiny.xnout signs-tiny.out
	timeout 8 ${BINDIR}/signs signs-tiny.in signs-tiny.xout
	diff signs-tiny.xout signs-tiny.out

${BINDIR}/xmut: obj.d/xmut.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

xmut-test: ${BINDIR}/xmut xmut-tiny.in xmut-tiny.out
	timeout 8 ${BINDIR}/xmut -naive xmut-tiny.in xmut-tiny.xnout
	diff xmut-tiny.xnout xmut-tiny.out
	timeout 8 ${BINDIR}/xmut xmut-tiny.in xmut-tiny.xout
	diff xmut-tiny.xout xmut-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
