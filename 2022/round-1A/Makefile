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

${BINDIR}/eqsum: obj.d/eqsum.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

EQSUM_DEBUG=0x0
EQSUM_TIMEOUT=6
EQSUM_TEST_REQ = ${BINDIR}/eqsum interactive_runner.py local_testing_tool.py
eqsum-test: ${EQSUM_TEST_REQ}
	timeout ${EQSUM_TIMEOUT} \
	python interactive_runner.py python local_testing_tool.py -- \
	./${BINDIR}/eqsum -debug ${EQSUM_DEBUG}

eqsum-server-test: ${BINDIR}/eqsum eqsum-b.in
	./${BINDIR}/eqsum -debug 0x800 eqsum-b.in

${BINDIR}/weights: obj.d/weights.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

weights-test: weights-test1 weights-test2
weights-test1: ${BINDIR}/weights weights-tiny.in weights-tiny.out
	timeout 12 ${BINDIR}/weights -naive weights-tiny.in weights-tiny.xnout
	diff weights-tiny.xnout weights-tiny.out
weights-test2: ${BINDIR}/weights weights-tiny.in weights-tiny.out
	timeout 2 ${BINDIR}/weights weights-tiny.in weights-tiny.xout
	diff weights-tiny.xout weights-tiny.out

weights-internal-test: ${BINDIR}/weights
	timeout 10 ${BINDIR}/weights test 12 1 4 1 4 4

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
