#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2022/September/18
# codingcompetitions.withgoogle.com/kickstart/round/00000000008cb0f5

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

${BINDIR}/coloring: obj.d/coloring.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

coloring-test: ${BINDIR}/coloring coloring-tiny.in coloring-tiny.out
	timeout 2 ${BINDIR}/coloring -naive coloring-tiny.in coloring-tiny.xnout
	diff coloring-tiny.xnout coloring-tiny.out
	timeout 2 ${BINDIR}/coloring coloring-tiny.in coloring-tiny.xout
	diff coloring-tiny.xout coloring-tiny.out

${BINDIR}/students: obj.d/students.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

students-test: ${BINDIR}/students students-tiny.in students-tiny.out
	timeout 2 ${BINDIR}/students -naive students-tiny.in students-tiny.xnout
	diff students-tiny.xnout students-tiny.out
	timeout 2 ${BINDIR}/students students-tiny.in students-tiny.xout
	diff students-tiny.xout students-tiny.out

${BINDIR}/matching: obj.d/matching.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

matching-test: ${BINDIR}/matching matching-tiny.in matching-tiny.out
	timeout 2 ${BINDIR}/matching -naive matching-tiny.in matching-tiny.xnout
	diff matching-tiny.xnout matching-tiny.out
	timeout 2 ${BINDIR}/matching matching-tiny.in matching-tiny.xout
	diff matching-tiny.xout matching-tiny.out

${BINDIR}/pizza: obj.d/pizza.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

pizza-test: pizza-test1 pizza-test2

pizza-test1: ${BINDIR}/pizza pizza-tiny.in pizza-tiny.out
	timeout 2 ${BINDIR}/pizza -naive pizza-tiny.in pizza-tiny.xnout
	diff pizza-tiny.xnout pizza-tiny.out
	timeout 2 ${BINDIR}/pizza pizza-tiny.in pizza-tiny.xout
	diff pizza-tiny.xout pizza-tiny.out

pizza-test2: ${BINDIR}/pizza pizza-tiny2.in pizza-tiny2.out
	timeout 2 ${BINDIR}/pizza -naive pizza-tiny2.in pizza-tiny2.xnout
	diff pizza-tiny2.xnout pizza-tiny2.out
	timeout 2 ${BINDIR}/pizza pizza-tiny2.in pizza-tiny2.xout
	diff pizza-tiny2.xout pizza-tiny2.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
