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

letters-internal-test: ${BINDIR}/letters
	timeout 4 ${BINDIR}/letters test 12 3 6 4 14

${BINDIR}/squary: obj.d/squary.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

squary-test: ${BINDIR}/squary squary-tiny.in squary-tiny.out
	timeout 2 ${BINDIR}/squary -naive squary-tiny.in squary-tiny.xnout
	diff squary-tiny.xnout squary-tiny.out
	timeout 2 ${BINDIR}/squary squary-tiny.in squary-tiny.xout
	diff squary-tiny.xout squary-tiny.out

squary-test2: ${BINDIR}/squary squary-tiny2.in squary-tiny2.out
	timeout 2 ${BINDIR}/squary squary-tiny2.in squary-tiny2.xnout
	diff -u squary-tiny2.xnout squary-tiny2.out || true

${BINDIR}/intranets: obj.d/intranets.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

intranets-test: ${BINDIR}/intranets intranets-tiny.in intranets-tiny.out
	timeout 2 ${BINDIR}/intranets -naive intranets-tiny.in intranets-tiny.xnout
	diff intranets-tiny.xnout intranets-tiny.out
	timeout 2 ${BINDIR}/intranets intranets-tiny.in intranets-tiny.xout
	diff intranets-tiny.xout intranets-tiny.out

intranets-internal-test: ${BINDIR}/intranets
	${BINDIR}/intranets test

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
