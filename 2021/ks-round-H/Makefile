#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# codingcompetitions.withgoogle.com/kickstart/round/0000000000435914

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

${BINDIR}/xstr: obj.d/xstr.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

xstr-test: ${BINDIR}/xstr xstr-tiny.in xstr-tiny.out
	timeout 2 ${BINDIR}/xstr -naive xstr-tiny.in xstr-tiny.xnout
	diff xstr-tiny.xnout xstr-tiny.out
	timeout 2 ${BINDIR}/xstr xstr-tiny.in xstr-tiny.xout
	diff xstr-tiny.xout xstr-tiny.out
	timeout 2 ${BINDIR}/xstr -naive xstr-tiny2.in xstr-tiny2.xnout
	diff xstr-tiny2.xnout xstr-tiny2.out
	timeout 2 ${BINDIR}/xstr xstr-tiny2.in xstr-tiny2.xout
	diff xstr-tiny2.xout xstr-tiny2.out

${BINDIR}/painter: obj.d/painter.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

painter-test: ${BINDIR}/painter painter-tiny.in painter-tiny.out
	timeout 2 ${BINDIR}/painter -naive painter-tiny.in painter-tiny.xnout
	diff painter-tiny.xnout painter-tiny.out
	timeout 2 ${BINDIR}/painter painter-tiny.in painter-tiny.xout
	diff painter-tiny.xout painter-tiny.out
	timeout 2 ${BINDIR}/painter -naive painter-tiny2.in painter-tiny2.xnout
	diff painter-tiny2.xnout painter-tiny2.out
	timeout 2 ${BINDIR}/painter painter-tiny2.in painter-tiny2.xout
	diff painter-tiny2.xout painter-tiny2.out

${BINDIR}/silly: obj.d/silly.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

silly-test: ${BINDIR}/silly silly-tiny.in silly-tiny.out
	timeout 2 ${BINDIR}/silly -naive silly-tiny.in silly-tiny.xnout
	diff silly-tiny.xnout silly-tiny.out
	timeout 2 ${BINDIR}/silly silly-tiny.in silly-tiny.xout
	diff silly-tiny.xout silly-tiny.out

${BINDIR}/depevents: obj.d/depevents.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

depevents-test: ${BINDIR}/depevents depevents-tiny.in depevents-tiny.out
	timeout 2 ${BINDIR}/depevents -naive depevents-tiny.in depevents-tiny.xnout
	diff depevents-tiny.xnout depevents-tiny.out
	timeout 2 ${BINDIR}/depevents depevents-tiny.in depevents-tiny.xout
	diff depevents-tiny.xout depevents-tiny.out

depevents-self-test: ${BINDIR}/depevents
	${BINDIR}/depevents test specific 2 0 1/10 1/10 0 0 1
	${BINDIR}/depevents test specific 3 0 0 1/2 4/5 0 4/5 0 1 2
	${BINDIR}/depevents test specific 3 0 0 1/2 4/5 4/5 0 0 1 2
	${BINDIR}/depevents test specific 3 0 0 1/10 1/2 3/5 3/5 1/10 1 2
	${BINDIR}/depevents test specific 3 0 1 1/10 1/2 3/5 3/5 1/10 1 2

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
