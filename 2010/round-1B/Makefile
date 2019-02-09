#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/April/28

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

${BINDIR}/ffix: obj.d/ffix.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

ffix-test: ${BINDIR}/ffix ffix-tiny.in ffix-tiny.out
	timeout 8 ${BINDIR}/ffix ffix-tiny.in ffix-tiny.xout
	diff ffix-tiny.xout ffix-tiny.out

${BINDIR}/chicks: obj.d/chicks.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

chicks-test: ${BINDIR}/chicks chicks-tiny.in chicks-tiny.out
	timeout 8 ${BINDIR}/chicks -naive chicks-tiny.in chicks-tiny.xout
	diff chicks-tiny.xout chicks-tiny.out
	timeout 8 ${BINDIR}/chicks chicks-tiny.in chicks-tiny.xout
	diff chicks-tiny.xout chicks-tiny.out

${BINDIR}/Cxxx: obj.d/Cxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Cxxx-test: ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.out
	timeout 8 ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.xout
	diff Cxxx-tiny.xout Cxxx-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	timeout 8 ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
