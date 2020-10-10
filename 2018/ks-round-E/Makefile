#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
#  codingcompetitions.withgoogle.com/kickstart/round/0000000000050ff5

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

SHELL=/bin/bash
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

${BINDIR}/boardgame: obj.d/boardgame.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

boardgame-test: ${BINDIR}/boardgame boardgame-tiny.in boardgame-tiny.out
	timeout 8 ${BINDIR}/boardgame -naive boardgame-tiny.in boardgame-tiny.xnout
	numdiff -r 0.000001 boardgame-tiny.xnout boardgame-tiny.out
	timeout 8 ${BINDIR}/boardgame boardgame-tiny.in boardgame-tiny.xout
	numdiff -r 0.000001 boardgame-tiny.xout boardgame-tiny.out

${BINDIR}/milktea: obj.d/milktea.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

milktea-test: ${BINDIR}/milktea milktea-tiny.in milktea-tiny.out
	timeout 8 ${BINDIR}/milktea -naive milktea-tiny.in milktea-tiny.xnout
	diff milktea-tiny.xnout milktea-tiny.out
	timeout 8 ${BINDIR}/milktea milktea-tiny.in milktea-tiny.xout
	diff milktea-tiny.xout milktea-tiny.out

${BINDIR}/yogurt: obj.d/yogurt.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

yogurt-test: ${BINDIR}/yogurt yogurt-tiny.in yogurt-tiny.out
	timeout 8 ${BINDIR}/yogurt -naive yogurt-tiny.in yogurt-tiny.xnout
	diff yogurt-tiny.xnout yogurt-tiny.out
	timeout 8 ${BINDIR}/yogurt yogurt-tiny.in yogurt-tiny.xout
	diff yogurt-tiny.xout yogurt-tiny.out

################################
# internal exersices

obj.d/kdsegtree.o: kdsegtree.h

${BINDIR}/kdsegtree: obj.d/kdsegtree.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

kdsegtree-test: ${BINDIR}/kdsegtree
	timeout 2 ${BINDIR}/kdsegtree 1 l 0 2 0 1 p 1
	timeout 2 ${BINDIR}/kdsegtree 1 l 0 2 p 1
	timeout 2 ${BINDIR}/kdsegtree 1 l 0 2 1 1 p 1
	timeout 2 ${BINDIR}/kdsegtree 1 l 2 5 0 3 p 4
	timeout 2 ${BINDIR}/kdsegtree 1 l 3 5 0 4 p 1
	timeout 2 ${BINDIR}/kdsegtree 1 l 0 1 1 2 p 1
	timeout 2 ${BINDIR}/kdsegtree 2 l 2 2 2 3 0 4 0 2 p 2 2
	timeout 2 ${BINDIR}/kdsegtree 2 l 1 2 1 1 0 2 2 2 1 1 0 3 p 1 2
	timeout 2 ${BINDIR}/kdsegtree 2 l 1 2 1 2 1 1 0 2 p 2 2
	timeout 2 ${BINDIR}/kdsegtree 2 l 0 3 2 2 2 2 1 3 p 2 1
	timeout 2 ${BINDIR}/kdsegtree 2 l 1 2 2 3 1 4 1 4 p 1 2
	timeout 2 ${BINDIR}/kdsegtree 2 l 0 5 1 4  5 7 5 6  7 8 6 7  8 9 5 7  5 7 4 8  0 9 5 9  2 8 8 9  4 5 5 7  p 6 9
	timeout 2 ${BINDIR}/kdsegtree 2 l 1 2 0 0 0 1 1 1 p 1 1
	timeout 2 ${BINDIR}/kdsegtree 2 l 0 6 5 6 2 6 3 6 3 6 2 6 s 0 6 5 6 p 3 5
	timeout 2 ${BINDIR}/kdsegtree 2 l 8 8 4 4 0 8 6 6 0 6 8 8 2 5 7 8 5 6 0 4 7 9 0 4 p 5 6
	timeout 2 ${BINDIR}/kdsegtree 2 l 6 8 3 3 5 8 3 5 8 8 2 6 p 7 5
	timeout 2 ${BINDIR}/kdsegtree 2 l 4 4 2 2 3 5 2 4 5 6 1 5 p 4 4
	timeout 2 ${BINDIR}/kdsegtree 2 l 2 3 1 2 0 4 4 4 2 4 0 1 p 3 4
	timeout 2 ${BINDIR}/kdsegtree 2 l 1 3 1 3 2 3 2 2 3 3 2 4 p 2 3
	timeout 2 ${BINDIR}/kdsegtree 2 l 0 3 1 2 2 3 0 3 1 2 1 1 p 1 2

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
