#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/April/14

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

${BINDIR}/waffle: obj.d/waffle.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

waffle-test: ${BINDIR}/waffle waffle-tiny.in waffle-tiny.out
	${BINDIR}/waffle waffle-tiny.in waffle-tiny.xout
	diff waffle-tiny.xout waffle-tiny.out

${BINDIR}/bitparty: obj.d/bitparty.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

bitparty-test: ${BINDIR}/bitparty bitparty-tiny.in bitparty-tiny.out
	timeout 5 ${BINDIR}/bitparty -naive bitparty-tiny.in bitparty-tiny.xnout
	diff bitparty-tiny.xnout bitparty-tiny.out
	timeout 3 ${BINDIR}/bitparty bitparty-tiny.in bitparty-tiny.xout
	diff bitparty-tiny.xout bitparty-tiny.out

${BINDIR}/edgy: obj.d/edgy.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

edgy-test: ${BINDIR}/edgy edgy-tiny.in edgy-tiny.out
	timeout 10 ${BINDIR}/edgy edgy-tiny.in edgy-tiny.xout
	diff edgy-tiny.xout edgy-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
