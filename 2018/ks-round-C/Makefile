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

${BINDIR}/alarm: obj.d/alarm.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

alarm-test: ${BINDIR}/alarm alarm-tiny.in alarm-tiny.out
	timeout 2 ${BINDIR}/alarm -naive alarm-tiny.in alarm-tiny.xnout
	diff alarm-tiny.xnout alarm-tiny.out
	timeout 2 ${BINDIR}/alarm alarm-tiny.in alarm-tiny.xout
	diff alarm-tiny.xout alarm-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
