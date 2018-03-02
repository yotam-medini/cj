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
CFLAGS = ${DBGFLAGS} -Wall -std=c++11 -fno-exceptions -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/crazyrows: obj.d/crazyrows.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

crazyrows-test: ${BINDIR}/crazyrows crazyrows-tiny.in crazyrows-tiny.out
	${BINDIR}/crazyrows -naive crazyrows-tiny.in crazyrows-tiny.xnout
	diff crazyrows-tiny.xnout crazyrows-tiny.out
	${BINDIR}/crazyrows crazyrows-tiny.in crazyrows-tiny.xout
	diff crazyrows-tiny.xout crazyrows-tiny.out

${BINDIR}/digging: obj.d/digging.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

digging-test: ${BINDIR}/digging digging-tiny.in digging-tiny.out
	${BINDIR}/digging digging-tiny.in digging-tiny.xout
	diff digging-tiny.xout digging-tiny.out

${BINDIR}/Cxxx: obj.d/Cxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Cxxx-test: ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.out
	${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.xout
	diff Cxxx-tiny.xout Cxxx-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
