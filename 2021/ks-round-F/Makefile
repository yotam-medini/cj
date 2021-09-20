#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/0000000000435bae

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

${BINDIR}/trash: obj.d/trash.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

trash-test: ${BINDIR}/trash trash-tiny.in trash-tiny.out
	timeout 2 ${BINDIR}/trash -naive trash-tiny.in trash-tiny.xnout
	diff trash-tiny.xnout trash-tiny.out
	timeout 2 ${BINDIR}/trash trash-tiny.in trash-tiny.xout
	diff trash-tiny.xout trash-tiny.out

${BINDIR}/festival: obj.d/festival.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

festival-test: ${BINDIR}/festival festival-tiny.in festival-tiny.out
	timeout 2 ${BINDIR}/festival -naive festival-tiny.in festival-tiny.xnout
	diff festival-tiny.xnout festival-tiny.out
	timeout 2 ${BINDIR}/festival festival-tiny.in festival-tiny.xout
	diff festival-tiny.xout festival-tiny.out

${BINDIR}/star: obj.d/star.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

star-test: ${BINDIR}/star star-tiny.in star-tiny.out
	timeout 2 ${BINDIR}/star -naive star-tiny.in star-tiny.xnout
	diff star-tiny.xnout star-tiny.out
	timeout 2 ${BINDIR}/star star-tiny.in star-tiny.xout
	diff star-tiny.xout star-tiny.out

${BINDIR}/graph: obj.d/graph.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

graph-test: ${BINDIR}/graph graph-tiny.in graph-tiny.out
	timeout 2 ${BINDIR}/graph -naive graph-tiny.in graph-tiny.xnout
	diff graph-tiny.xnout graph-tiny.out
	timeout 2 ${BINDIR}/graph graph-tiny.in graph-tiny.xout
	diff graph-tiny.xout graph-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
