#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

DBGFLAGS = -g
OPTFLAGS = ${DBGFLAGS}
CFLAGS = ${OPTFLAGS} -Wall -Wshadow -std=c++14 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/pylon: obj.d/pylon.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

pylon-test: ${BINDIR}/pylon pylon-tiny.in pylon-tiny.out
	timeout 8 ${BINDIR}/pylon -naive pylon-tiny.in pylon-tiny.xnout
	diff pylon-tiny.xnout pylon-tiny.out
	timeout 8 ${BINDIR}/pylon pylon-tiny.in pylon-tiny.xout
	diff pylon-tiny.xout pylon-tiny.out

${BINDIR}/golf: obj.d/golf.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

golf-test: ${BINDIR}/golf golf-tiny.in golf-tiny.out
	timeout 8 ${BINDIR}/golf golf-tiny.in golf-tiny.xout
	diff golf-tiny.xout golf-tiny.out

${BINDIR}/Cxxx: obj.d/Cxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Cxxx-test: ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.out
	timeout 8 ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.xout
	diff Cxxx-tiny.xout Cxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
