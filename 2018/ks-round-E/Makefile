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

${BINDIR}/Cxxx: obj.d/Cxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Cxxx-test: ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.out
	timeout 8 ${BINDIR}/Cxxx -naive Cxxx-tiny.in Cxxx-tiny.xnout
	diff Cxxx-tiny.xnout Cxxx-tiny.out
	timeout 8 ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.xout
	diff Cxxx-tiny.xout Cxxx-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	timeout 8 ${BINDIR}/Dxxx -naive Dxxx-tiny.in Dxxx-tiny.xnout
	diff Dxxx-tiny.xnout Dxxx-tiny.out
	timeout 8 ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
