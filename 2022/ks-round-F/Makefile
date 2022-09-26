#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2022/September/25
# codingcompetitions.withgoogle.com/kickstart/round/00000000008cb409

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

${BINDIR}/fabric: obj.d/fabric.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

fabric-test: fabric-test1 fabric-test2

fabric-test1: ${BINDIR}/fabric fabric-tiny.in fabric-tiny.out
	timeout 2 ${BINDIR}/fabric -naive fabric-tiny.in fabric-tiny.xnout
	diff fabric-tiny.xnout fabric-tiny.out
	timeout 2 ${BINDIR}/fabric fabric-tiny.in fabric-tiny.xout
	diff fabric-tiny.xout fabric-tiny.out

fabric-test2: ${BINDIR}/fabric fabric-tiny2.in fabric-tiny2.out
	timeout 2 ${BINDIR}/fabric -naive fabric-tiny2.in fabric-tiny2.xnout
	diff fabric-tiny2.xnout fabric-tiny2.out
	timeout 2 ${BINDIR}/fabric fabric-tiny2.in fabric-tiny2.xout
	diff fabric-tiny2.xout fabric-tiny2.out

${BINDIR}/water: obj.d/water.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

water-test: water-test1 water-test2

water-test1: ${BINDIR}/water water-tiny.in water-tiny.out
	timeout 2 ${BINDIR}/water -naive water-tiny.in water-tiny.xnout
	diff water-tiny.xnout water-tiny.out
	timeout 2 ${BINDIR}/water water-tiny.in water-tiny.xout
	diff water-tiny.xout water-tiny.out

water-test2: ${BINDIR}/water water-tiny2.in water-tiny2.out
	timeout 2 ${BINDIR}/water -naive water-tiny2.in water-tiny2.xnout
	diff water-tiny2.xnout water-tiny2.out
	timeout 2 ${BINDIR}/water water-tiny2.in water-tiny2.xout
	diff water-tiny2.xout water-tiny2.out

${BINDIR}/seasons: obj.d/seasons.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

seasons-test: seasons-test1 seasons-test2

seasons-test1: ${BINDIR}/seasons seasons-tiny.in seasons-tiny.out
	timeout 2 ${BINDIR}/seasons -naive seasons-tiny.in seasons-tiny.xnout
	diff seasons-tiny.xnout seasons-tiny.out
	timeout 2 ${BINDIR}/seasons seasons-tiny.in seasons-tiny.xout
	diff seasons-tiny.xout seasons-tiny.out

seasons-test2: ${BINDIR}/seasons seasons-tiny2.in seasons-tiny2.out
	timeout 2 ${BINDIR}/seasons -naive seasons-tiny2.in seasons-tiny2.xnout
	diff seasons-tiny2.xnout seasons-tiny2.out
	timeout 2 ${BINDIR}/seasons seasons-tiny2.in seasons-tiny2.xout
	diff seasons-tiny2.xout seasons-tiny2.out

${BINDIR}/meeting: obj.d/meeting.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

meeting-test: ${BINDIR}/meeting meeting-tiny.in meeting-tiny.out
	timeout 2 ${BINDIR}/meeting -naive meeting-tiny.in meeting-tiny.xnout
	diff meeting-tiny.xnout meeting-tiny.out
	timeout 2 ${BINDIR}/meeting meeting-tiny.in meeting-tiny.xout
	diff meeting-tiny.xout meeting-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
