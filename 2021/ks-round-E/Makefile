#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/000000000043585c

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

SHELL=/bin/bash
DBGFLAGS = -g
OPTFLAGS = ${DBGFLAGS}
CFLAGS = ${OPTFLAGS} -Wall -Wshadow -std=c++17 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/sanagram: obj.d/sanagram.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

sanagram-test: ${BINDIR}/sanagram sanagram-tiny.in sanagram-tiny.out
	timeout 2 ${BINDIR}/sanagram -naive sanagram-tiny.in sanagram-tiny.xnout
	diff <(grep IMPOSSIBLE sanagram-tiny.xnout) <(grep IMPOSSIBLE sanagram-tiny.out)	
	timeout 2 ${BINDIR}/sanagram sanagram-tiny.in sanagram-tiny.xout
	diff <(grep IMPOSSIBLE sanagram-tiny.xout) <(grep IMPOSSIBLE sanagram-tiny.out)


${BINDIR}/bcake: obj.d/bcake.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

bcake-test: bcake-test1 bcake-test2

bcake-test1: ${BINDIR}/bcake bcake-tiny.in bcake-tiny.out
	timeout 2 ${BINDIR}/bcake -naive bcake-tiny.in bcake-tiny.xnout
	diff bcake-tiny.xnout bcake-tiny.out
	timeout 2 ${BINDIR}/bcake bcake-tiny.in bcake-tiny.xout
	diff bcake-tiny.xout bcake-tiny.out

bcake-test2: ${BINDIR}/bcake bcake-tiny2.in bcake-tiny2.out
	timeout 2 ${BINDIR}/bcake -naive bcake-tiny2.in bcake-tiny2.xnout
	diff bcake-tiny2.xnout bcake-tiny2.out
	timeout 2 ${BINDIR}/bcake bcake-tiny2.in bcake-tiny2.xout
	diff bcake-tiny2.xout bcake-tiny2.out

${BINDIR}/palicross: obj.d/palicross.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

palicross-test: ${BINDIR}/palicross palicross-tiny.in palicross-tiny.out
	timeout 2 ${BINDIR}/palicross -naive palicross-tiny.in palicross-tiny.xnout
	diff palicross-tiny.xnout palicross-tiny.out
	timeout 2 ${BINDIR}/palicross palicross-tiny.in palicross-tiny.xout
	diff palicross-tiny.xout palicross-tiny.out

${BINDIR}/incr: obj.d/incr.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

incr-test: ${BINDIR}/incr incr-tiny.in incr-tiny.out
	timeout 2 ${BINDIR}/incr -naive incr-tiny.in incr-tiny.xnout
	numdiff -r 0.000001 incr-tiny.xnout incr-tiny.out
	timeout 2 ${BINDIR}/incr incr-tiny.in incr-tiny.xout
	numdiff -r 0.000001 incr-tiny.xout incr-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
