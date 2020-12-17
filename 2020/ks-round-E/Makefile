#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# https://codingcompetitions.withgoogle.com/kickstart/round/000000000019ff47

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

${BINDIR}/longarith: obj.d/longarith.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

longarith-test: ${BINDIR}/longarith longarith-tiny.in longarith-tiny.out
	timeout 8 ${BINDIR}/longarith -naive longarith-tiny.in longarith-tiny.xnout
	diff longarith-tiny.xnout longarith-tiny.out
	timeout 8 ${BINDIR}/longarith longarith-tiny.in longarith-tiny.xout
	diff longarith-tiny.xout longarith-tiny.out

${BINDIR}/highbuild: obj.d/highbuild.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

highbuild-test: ${BINDIR}/highbuild highbuild-tiny.in highbuild-tiny.out
	timeout 4 ${BINDIR}/highbuild -naive highbuild-tiny.in highbuild-tiny.xnout
	diff <(grep IMPOSSIBLE highbuild-tiny.xnout) <(grep IMPOSSIBLE highbuild-tiny.out)
	timeout 4 ${BINDIR}/highbuild highbuild-tiny.in highbuild-tiny.xout
	diff <(grep IMPOSSIBLE highbuild-tiny.xout) <(grep IMPOSSIBLE highbuild-tiny.out)

${BINDIR}/toys: obj.d/toys.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

toys-test: ${BINDIR}/toys toys-tiny.in toys-tiny.out
	timeout 4 ${BINDIR}/toys -naive toys-tiny.in toys-tiny.xnout
	diff toys-tiny.xnout toys-tiny.out
	timeout 4 ${BINDIR}/toys toys-tiny.in toys-tiny.xout
	diff toys-tiny.xout toys-tiny.out

${BINDIR}/goldstone: obj.d/goldstone.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

goldstone-test: ${BINDIR}/goldstone goldstone-tiny.in goldstone-tiny.out
	timeout 4 ${BINDIR}/goldstone -naive goldstone-tiny.in goldstone-tiny.xnout
	diff goldstone-tiny.xnout goldstone-tiny.out
	timeout 4 ${BINDIR}/goldstone goldstone-tiny.in goldstone-tiny.xout
	diff goldstone-tiny.xout goldstone-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
