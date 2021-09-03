#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# codingcompetitions.withgoogle.com/codejam/round/00000000004362d7

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

${BINDIR}/pick: obj.d/pick.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

pick-test: ${BINDIR}/pick pick-tiny.in pick-tiny.out
	timeout 2 ${BINDIR}/pick -naive pick-tiny.in pick-tiny.xnout
	numdiff -r 0.000001 pick-tiny.xnout pick-tiny.out
	timeout 2 ${BINDIR}/pick pick-tiny.in pick-tiny.xout
	numdiff -r 0.000001 pick-tiny.xout pick-tiny.out

${BINDIR}/years: obj.d/years.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

years-test: ${BINDIR}/years years-tiny.in years-tiny.out
	timeout 2 ${BINDIR}/years -naive years-tiny.in years-tiny.xnout
	diff years-tiny.xnout years-tiny.out
	timeout 2 ${BINDIR}/years years-tiny.in years-tiny.xout
	diff years-tiny.xout years-tiny.out

${BINDIR}/doubleornot: obj.d/doubleornot.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

doubleornot-test: ${BINDIR}/doubleornot doubleornot-tiny.in doubleornot-tiny.out
	timeout 2 ${BINDIR}/doubleornot -naive doubleornot-tiny.in doubleornot-tiny.xnout
	diff doubleornot-tiny.xnout doubleornot-tiny.out
	timeout 2 ${BINDIR}/doubleornot doubleornot-tiny.in doubleornot-tiny.xout
	diff doubleornot-tiny.xout doubleornot-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
