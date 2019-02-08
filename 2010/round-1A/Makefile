#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/April/13

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

${BINDIR}/rotate: obj.d/rotate.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

rotate-test: ${BINDIR}/rotate rotate-tiny.in rotate-tiny.out
	${BINDIR}/rotate rotate-tiny.in rotate-tiny.xout
	diff rotate-tiny.xout rotate-tiny.out

${BINDIR}/smooth: obj.d/smooth.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

smooth-test: ${BINDIR}/smooth smooth-tiny.in smooth-tiny.out
	timeout 3 ${BINDIR}/smooth -naive smooth-tiny.in smooth-tiny.xnout
	diff smooth-tiny.xnout smooth-tiny.out
	timeout 3 ${BINDIR}/smooth smooth-tiny.in smooth-tiny.xout
	diff smooth-tiny.xout smooth-tiny.out

${BINDIR}/numgame: obj.d/numgame.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

numgame-test: ${BINDIR}/numgame numgame-tiny.in numgame-tiny.out
	${BINDIR}/numgame -naive numgame-tiny.in numgame-tiny.xout
	diff numgame-tiny.xout numgame-tiny.out
	${BINDIR}/numgame numgame-tiny.in numgame-tiny.xout
	diff numgame-tiny.xout numgame-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
