#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/March/23
#  https://code.google.com/codejam/contest/5374486/dashboard
#  codingcompetitions.withgoogle.com/kickstart/round/0000000000051066

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

${BINDIR}/triplets: obj.d/triplets.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

triplets-test: ${BINDIR}/triplets triplets-tiny.in triplets-tiny.out
	timeout 8 ${BINDIR}/triplets -naive triplets-tiny.in triplets-tiny.xnout
	diff triplets-tiny.xnout triplets-tiny.out
	timeout 8 ${BINDIR}/triplets triplets-tiny.in triplets-tiny.xout
	diff triplets-tiny.xout triplets-tiny.out

${BINDIR}/combcls: obj.d/combcls.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

combcls-test: ${BINDIR}/combcls combcls-tiny.in combcls-tiny.out
	timeout 8 ${BINDIR}/combcls -naive combcls-tiny.in combcls-tiny.xnout
	diff combcls-tiny.xnout combcls-tiny.out
	timeout 8 ${BINDIR}/combcls combcls-tiny.in combcls-tiny.xout
	diff combcls-tiny.xout combcls-tiny.out

${BINDIR}/cave: obj.d/cave.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

cave-test: ${BINDIR}/cave cave-tiny.in cave-tiny.out
	timeout 8 ${BINDIR}/cave cave-tiny.in cave-tiny.xout
	diff cave-tiny.xout cave-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
