#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/00000000001a0069

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

${BINDIR}/kicks: obj.d/kicks.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

kicks-test: ${BINDIR}/kicks kicks-tiny.in kicks-tiny.out
	timeout 2 ${BINDIR}/kicks -naive kicks-tiny.in kicks-tiny.xnout
	diff kicks-tiny.xnout kicks-tiny.out
	timeout 2 ${BINDIR}/kicks kicks-tiny.in kicks-tiny.xout
	diff kicks-tiny.xout kicks-tiny.out

${BINDIR}/coins: obj.d/coins.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

coins-test: ${BINDIR}/coins coins-tiny.in coins-tiny.out
	timeout 8 ${BINDIR}/coins -naive coins-tiny.in coins-tiny.xnout
	diff coins-tiny.xnout coins-tiny.out
	timeout 8 ${BINDIR}/coins coins-tiny.in coins-tiny.xout
	diff coins-tiny.xout coins-tiny.out

${BINDIR}/lock: obj.d/lock.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

lock-test: ${BINDIR}/lock lock-tiny.in lock-tiny.out
	timeout 4 ${BINDIR}/lock -naive lock-tiny.in lock-tiny.xnout
	diff lock-tiny.xnout lock-tiny.out
	timeout 4 ${BINDIR}/lock lock-tiny.in lock-tiny.xout
	diff lock-tiny.xout lock-tiny.out

${BINDIR}/cards: obj.d/cards.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

cards-test: ${BINDIR}/cards cards-tiny.in cards-tiny.out
	timeout 8 ${BINDIR}/cards -naive cards-tiny.in cards-tiny.xnout
	numdiff -r 0.00001 cards-tiny.xnout cards-tiny.out
	timeout 8 ${BINDIR}/cards cards-tiny.in cards-tiny.xout
	numdiff -r 0.00001 cards-tiny.xout cards-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
