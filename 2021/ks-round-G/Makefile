#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/00000000004362d6/

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

${BINDIR}/dog: obj.d/dog.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

dog-test: ${BINDIR}/dog dog-tiny.in dog-tiny.out
	timeout 2 ${BINDIR}/dog -naive dog-tiny.in dog-tiny.xnout
	diff dog-tiny.xnout dog-tiny.out
	timeout 2 ${BINDIR}/dog dog-tiny.in dog-tiny.xout
	diff dog-tiny.xout dog-tiny.out
	timeout 2 ${BINDIR}/dog -naive dog-tiny2.in dog-tiny2.xnout
	diff dog-tiny2.xnout dog-tiny2.out
	timeout 2 ${BINDIR}/dog dog-tiny2.in dog-tiny2.xout
	diff dog-tiny2.xout dog-tiny2.out

${BINDIR}/hydrated: obj.d/hydrated.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

hydrated-test: ${BINDIR}/hydrated hydrated-tiny.in hydrated-tiny.out
	timeout 2 ${BINDIR}/hydrated -naive hydrated-tiny.in hydrated-tiny.xnout
	diff hydrated-tiny.xnout hydrated-tiny.out
	timeout 2 ${BINDIR}/hydrated hydrated-tiny.in hydrated-tiny.xout
	diff hydrated-tiny.xout hydrated-tiny.out

${BINDIR}/banana: obj.d/banana.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

banana-test: ${BINDIR}/banana banana-tiny.in banana-tiny.out
	timeout 2 ${BINDIR}/banana -naive banana-tiny.in banana-tiny.xnout
	diff banana-tiny.xnout banana-tiny.out
	timeout 2 ${BINDIR}/banana banana-tiny.in banana-tiny.xout
	diff banana-tiny.xout banana-tiny.out

${BINDIR}/polygon: obj.d/polygon.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

polygon-test: ${BINDIR}/polygon polygon-tiny.in polygon-tiny.out
	timeout 2 ${BINDIR}/polygon -naive polygon-tiny.in polygon-tiny.xnout
	diff polygon-tiny.xnout polygon-tiny.out
	timeout 2 ${BINDIR}/polygon polygon-tiny.in polygon-tiny.xout
	diff polygon-tiny.xout polygon-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
