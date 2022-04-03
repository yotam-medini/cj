#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/codejam/round/0000000000876ff1

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

${BINDIR}/punched: obj.d/punched.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

punched-test: ${BINDIR}/punched punched-tiny.in punched-tiny.out
	timeout 2 ${BINDIR}/punched -naive punched-tiny.in punched-tiny.xnout
	diff punched-tiny.xnout punched-tiny.out
	timeout 2 ${BINDIR}/punched punched-tiny.in punched-tiny.xout
	diff punched-tiny.xout punched-tiny.out

${BINDIR}/threed: obj.d/threed.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

threed-test: ${BINDIR}/threed threed-tiny.in threed-tiny.out
	timeout 2 ${BINDIR}/threed -naive threed-tiny.in threed-tiny.xnout
	diff <(grep POSSIBLE threed-tiny.xnout) <(grep POSSIBLE threed-tiny.out)
	timeout 2 ${BINDIR}/threed threed-tiny.in threed-tiny.xout
	diff <(grep POSSIBLE threed-tiny.xout) <(grep POSSIBLE threed-tiny.out)


${BINDIR}/dices: obj.d/dices.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

dices-test: ${BINDIR}/dices dices-tiny.in dices-tiny.out
	timeout 2 ${BINDIR}/dices -naive dices-tiny.in dices-tiny.xnout
	diff dices-tiny.xnout dices-tiny.out
	timeout 2 ${BINDIR}/dices dices-tiny.in dices-tiny.xout
	diff dices-tiny.xout dices-tiny.out

${BINDIR}/chain: obj.d/chain.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

chain-test: ${BINDIR}/chain chain-tiny.in chain-tiny.out
	timeout 2 ${BINDIR}/chain -naive chain-tiny.in chain-tiny.xnout
	diff chain-tiny.xnout chain-tiny.out
	timeout 2 ${BINDIR}/chain chain-tiny.in chain-tiny.xout
	diff chain-tiny.xout chain-tiny.out

${BINDIR}/twisty: obj.d/twisty.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

TWISTY_DEBUG=0x1
TWISTY_TIMEOUT=6
TWISTY_TEST_REQ = ${BINDIR}/twisty interactive_runner.py local_testing_tool.py
# TWISTY_TEST_REQ = ${BINDIR}/twisty interactive_runner.py my_testing_tool.py
twisty-test: ${TWISTY_TEST_REQ}
	timeout ${TWISTY_TIMEOUT} python interactive_runner.py \
	  python my_testing_tool.py $(2) -- \
	  ./${BINDIR}/twisty -debug ${TWISTY_DEBUG}

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
