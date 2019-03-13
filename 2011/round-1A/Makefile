#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/February/23

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

${BINDIR}/fsstats: obj.d/fsstats.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

fsstats-test: ${BINDIR}/fsstats fsstats-tiny.in fsstats-tiny.out
	timeout 8 ${BINDIR}/fsstats fsstats-tiny.in fsstats-tiny.xout
	diff fsstats-tiny.xout fsstats-tiny.out

${BINDIR}/killword: obj.d/killword.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

killword-test: ${BINDIR}/killword killword-tiny.in killword-tiny.out
	timeout 8 ${BINDIR}/killword -naive killword-tiny.in killword-tiny.xout
	diff killword-tiny.xout killword-tiny.out
	timeout 8 ${BINDIR}/killword killword-tiny.in killword-tiny.xout
	diff killword-tiny.xout killword-tiny.out

${BINDIR}/pminion: obj.d/pminion.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

pminion-test: ${BINDIR}/pminion pminion-tiny.in pminion-tiny.out
	timeout 8 ${BINDIR}/pminion pminion-tiny.in pminion-tiny.xout
	diff pminion-tiny.xout pminion-tiny.out
	timeout 8 ${BINDIR}/pminion pminion-tiny.in pminion-tiny.xout
	diff pminion-tiny.xout pminion-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
