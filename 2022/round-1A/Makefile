#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# /codingcompetitions.withgoogle.com/codejam/round/0000000000877ba5

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

${BINDIR}/double: obj.d/double.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

double-test: ${BINDIR}/double double-tiny.in double-tiny.out
	timeout 2 ${BINDIR}/double -naive double-tiny.in double-tiny.xnout
	diff double-tiny.xnout double-tiny.out
	timeout 2 ${BINDIR}/double double-tiny.in double-tiny.xout
	diff double-tiny.xout double-tiny.out

${BINDIR}/Bxxx: obj.d/Bxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Bxxx-test: ${BINDIR}/Bxxx Bxxx-tiny.in Bxxx-tiny.out
	timeout 2 ${BINDIR}/Bxxx -naive Bxxx-tiny.in Bxxx-tiny.xnout
	diff Bxxx-tiny.xnout Bxxx-tiny.out
	timeout 2 ${BINDIR}/Bxxx Bxxx-tiny.in Bxxx-tiny.xout
	diff Bxxx-tiny.xout Bxxx-tiny.out

${BINDIR}/weights: obj.d/weights.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

weights-test: ${BINDIR}/weights weights-tiny.in weights-tiny.out
	timeout 12 ${BINDIR}/weights -naive weights-tiny.in weights-tiny.xnout
	diff weights-tiny.xnout weights-tiny.out
	timeout 2 ${BINDIR}/weights weights-tiny.in weights-tiny.xout
	diff weights-tiny.xout weights-tiny.out

weights-internal-test: ${BINDIR}/weights
	timeout 10 ${BINDIR}/weights test 12 1 4 1 4 4

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
