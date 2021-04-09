#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# codingcompetitions.withgoogle.com/kickstart/round/0000000000050ee0
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

${BINDIR}/planet: obj.d/planet.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

planet-test: ${BINDIR}/planet planet-tiny.in planet-tiny.out
	timeout 2 ${BINDIR}/planet -naive planet-tiny.in planet-tiny.xnout
	diff planet-tiny.xnout planet-tiny.out
	timeout 2 ${BINDIR}/planet planet-tiny.in planet-tiny.xout
	diff planet-tiny.xout planet-tiny.out

${BINDIR}/fairies: obj.d/fairies.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

fairies-test: ${BINDIR}/fairies fairies-tiny.in fairies-tiny.out
	timeout 2 ${BINDIR}/fairies -naive fairies-tiny.in fairies-tiny.xnout
	diff fairies-tiny.xnout fairies-tiny.out
	timeout 2 ${BINDIR}/fairies fairies-tiny.in fairies-tiny.xout
	diff fairies-tiny.xout fairies-tiny.out

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
