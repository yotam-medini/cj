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

${BINDIR}/lastword: obj.d/lastword.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

lastword-test: ${BINDIR}/lastword lastword-tiny.in lastword-tiny.out
	${BINDIR}/lastword lastword-tiny.in lastword-tiny.xout
	diff lastword-tiny.xout lastword-tiny.out

${BINDIR}/rank: obj.d/rank.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

rank-test: ${BINDIR}/rank rank-tiny.in rank-tiny.out
	${BINDIR}/rank rank-tiny.in rank-tiny.xout
	diff rank-tiny.xout rank-tiny.out

${BINDIR}/bff: obj.d/bff.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

bff-test: ${BINDIR}/bff bff-tiny.in bff-tiny.out
	${BINDIR}/bff bff-tiny.in bff-tiny.xout
	diff bff-tiny.xout bff-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}


