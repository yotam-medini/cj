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

${BINDIR}/decision: obj.d/decision.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

decision-test: ${BINDIR}/decision decision-tiny.in decision-tiny.out
	${BINDIR}/decision decision-tiny.in decision-tiny.xout
	diff decision-tiny.xout decision-tiny.out

${BINDIR}/nextnum: obj.d/nextnum.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

nextnum-test: ${BINDIR}/nextnum nextnum-tiny.in nextnum-tiny.out
	${BINDIR}/nextnum nextnum-tiny.in nextnum-tiny.xout
	diff nextnum-tiny.xout nextnum-tiny.out

${BINDIR}/squaremath: obj.d/squaremath.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

squaremath-test: ${BINDIR}/squaremath squaremath-tiny.in squaremath-tiny.out
	${BINDIR}/squaremath squaremath-tiny.in squaremath-tiny.xout
	diff squaremath-tiny.xout squaremath-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
