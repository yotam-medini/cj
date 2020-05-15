#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/codejam/round/000000000019fef2

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

${BINDIR}/expogo: obj.d/expogo.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

expogo-test: ${BINDIR}/expogo expogo-tiny.in expogo-tiny.out
	timeout 2 ${BINDIR}/expogo -naive expogo-tiny.in expogo-tiny.xnout
	diff expogo-tiny.xnout expogo-tiny.out
	timeout 2 ${BINDIR}/expogo expogo-tiny.in expogo-tiny.xout
	diff expogo-tiny.xout expogo-tiny.out

${BINDIR}/blind: obj.d/blind.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

clean-extcases:
	rm -f extcases.py extcases.pyc

# Consider appending  -debug 0x1  (after ./${BINDIR}/blind)
blind-test-0: ${BINDIR}/blind interactive_runner.py testing_tool.py
	timeout 4 python interactive_runner.py \
	  ./my-testing_tool.py 0 -- ./${BINDIR}/blind
blind-test-1: ${BINDIR}/blind interactive_runner.py testing_tool.py
	timeout 4 python interactive_runner.py \
	  ./my-testing_tool.py 1 -- ./${BINDIR}/blind
blind-test-2: ${BINDIR}/blind interactive_runner.py testing_tool.py
	timeout 4 python interactive_runner.py \
	  ./my-testing_tool.py 2 -- ./${BINDIR}/blind

blind-test: clean-extcases blind-test-0 blind-test-1 blind-test-2

${BINDIR}/ranks: obj.d/ranks.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

ranks-test: ${BINDIR}/ranks ranks-tiny.in ranks-tiny.out
	timeout 8 ${BINDIR}/ranks -naive ranks-tiny.in ranks-tiny.xnout
	diff <(grep Case ranks-tiny.xnout) <(grep Case ranks-tiny.out)
	timeout 8 ${BINDIR}/ranks ranks-tiny.in ranks-tiny.xout
	diff <(grep Case ranks-tiny.xout) <(grep Case ranks-tiny.out)

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
