#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# codingcompetitions.withgoogle.com/codejam/round/000000000087711b

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

${BINDIR}/pancake: obj.d/pancake.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

pancake-test: ${BINDIR}/pancake pancake-tiny.in pancake-tiny.out
	timeout 2 ${BINDIR}/pancake -naive pancake-tiny.in pancake-tiny.xnout
	diff pancake-tiny.xnout pancake-tiny.out
	timeout 2 ${BINDIR}/pancake pancake-tiny.in pancake-tiny.xout
	diff pancake-tiny.xout pancake-tiny.out

${BINDIR}/inflate: obj.d/inflate.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

inflate-test: ${BINDIR}/inflate inflate-tiny.in inflate-tiny.out
	timeout 2 ${BINDIR}/inflate -naive inflate-tiny.in inflate-tiny.xnout
	diff inflate-tiny.xnout inflate-tiny.out
	timeout 2 ${BINDIR}/inflate inflate-tiny.in inflate-tiny.xout
	diff inflate-tiny.xout inflate-tiny.out

${BINDIR}/asedatab: obj.d/asedatab.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

ASEDATAB_DEBUG=0x0
ASEDATAB_TIMEOUT=6
asedatab-test: ${BINDIR}/asedatab interactive_runner.py local_testing_tool.py
	timeout ${ASEDATAB_TIMEOUT} \
	python interactive_runner.py python local_testing_tool.py -- \
	./${BINDIR}/asedatab -debug ${ASEDATAB_DEBUG}

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
