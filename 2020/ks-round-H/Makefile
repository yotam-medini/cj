#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/000000000019ff49

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

${BINDIR}/retype: obj.d/retype.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

retype-test: ${BINDIR}/retype retype-tiny.in retype-tiny.out
	timeout 3 ${BINDIR}/retype -naive retype-tiny.in retype-tiny.xnout
	diff retype-tiny.xnout retype-tiny.out
	timeout 3 ${BINDIR}/retype retype-tiny.in retype-tiny.xout
	diff retype-tiny.xout retype-tiny.out

${BINDIR}/boring: obj.d/boring.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

boring-test: ${BINDIR}/boring boring-tiny.in boring-tiny.out
	timeout 3 ${BINDIR}/boring -naive boring-tiny.in boring-tiny.xnout
	diff boring-tiny.xnout boring-tiny.out
	timeout 3 ${BINDIR}/boring boring-tiny.in boring-tiny.xout
	diff boring-tiny.xout boring-tiny.out

${BINDIR}/Cxxx: obj.d/Cxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Cxxx-test: ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.out
	timeout 3 ${BINDIR}/Cxxx -naive Cxxx-tiny.in Cxxx-tiny.xnout
	diff Cxxx-tiny.xnout Cxxx-tiny.out
	timeout 3 ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.xout
	diff Cxxx-tiny.xout Cxxx-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	timeout 3 ${BINDIR}/Dxxx -naive Dxxx-tiny.in Dxxx-tiny.xnout
	diff Dxxx-tiny.xnout Dxxx-tiny.out
	timeout 3 ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
