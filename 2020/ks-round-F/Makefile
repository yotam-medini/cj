#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/000000000019ff48

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

${BINDIR}/atm: obj.d/atm.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

atm-test: ${BINDIR}/atm atm-tiny.in atm-tiny.out
	timeout 2 ${BINDIR}/atm -naive atm-tiny.in atm-tiny.xnout
	diff atm-tiny.xnout atm-tiny.out
	timeout 2 ${BINDIR}/atm atm-tiny.in atm-tiny.xout
	diff atm-tiny.xout atm-tiny.out

${BINDIR}/harvest: obj.d/harvest.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

harvest-test: ${BINDIR}/harvest harvest-tiny.in harvest-tiny.out
	timeout 8 ${BINDIR}/harvest -naive harvest-tiny.in harvest-tiny.xnout
	diff harvest-tiny.xnout harvest-tiny.out
	timeout 8 ${BINDIR}/harvest harvest-tiny.in harvest-tiny.xout
	diff harvest-tiny.xout harvest-tiny.out

${BINDIR}/duel: obj.d/duel.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

duel-test: duel-test-tiny duel-test-tiny2

duel-test-tiny: ${BINDIR}/duel duel-tiny.in duel-tiny.out
	timeout 2 ${BINDIR}/duel -naive duel-tiny.in duel-tiny.xnout
	diff duel-tiny.xnout duel-tiny.out
	timeout 2 ${BINDIR}/duel duel-tiny.in duel-tiny.xout
	diff duel-tiny.xout duel-tiny.out

duel-test-tiny2: ${BINDIR}/duel duel-tiny2.in duel-tiny2.out
	timeout 4 ${BINDIR}/duel duel-tiny2.in duel-tiny2.xout
	diff duel-tiny2.xout duel-tiny2.out

${BINDIR}/yeetzhee: obj.d/yeetzhee.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

yeetzhee-test: ${BINDIR}/yeetzhee yeetzhee-tiny.in yeetzhee-tiny.out
	timeout 8 ${BINDIR}/yeetzhee -naive yeetzhee-tiny.in yeetzhee-tiny.xnout
	numdiff -r 0.00001 yeetzhee-tiny.xnout yeetzhee-tiny.out
	timeout 8 ${BINDIR}/yeetzhee yeetzhee-tiny.in yeetzhee-tiny.xout
	numdiff -r 0.00001 yeetzhee-tiny.xout yeetzhee-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
