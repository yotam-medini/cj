#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/codejam/round/0000000000877b42

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

${BINDIR}/letters: obj.d/letters.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

letters-test: ${BINDIR}/letters letters-tiny.in letters-tiny.out
	timeout 2 ${BINDIR}/letters -naive letters-tiny.in letters-tiny.xnout
	diff <(grep IMPOSSIBLE letters-tiny.xnout) <(grep IMPOSSIBLE letters-tiny.out)
	timeout 2 ${BINDIR}/letters letters-tiny.in letters-tiny.xout
	diff <(grep IMPOSSIBLE letters-tiny.xout) <(grep IMPOSSIBLE letters-tiny.out)

${BINDIR}/squary: obj.d/squary.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

squary-test: ${BINDIR}/squary squary-tiny.in squary-tiny.out
	timeout 2 ${BINDIR}/squary -naive squary-tiny.in squary-tiny.xnout
	diff squary-tiny.xnout squary-tiny.out
	timeout 2 ${BINDIR}/squary squary-tiny.in squary-tiny.xout
	diff squary-tiny.xout squary-tiny.out

${BINDIR}/Cxxx: obj.d/Cxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Cxxx-test: ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.out
	timeout 2 ${BINDIR}/Cxxx -naive Cxxx-tiny.in Cxxx-tiny.xnout
	diff Cxxx-tiny.xnout Cxxx-tiny.out
	timeout 2 ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.xout
	diff Cxxx-tiny.xout Cxxx-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	timeout 2 ${BINDIR}/Dxxx -naive Dxxx-tiny.in Dxxx-tiny.xnout
	diff Dxxx-tiny.xnout Dxxx-tiny.out
	timeout 2 ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
