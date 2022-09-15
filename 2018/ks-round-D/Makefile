#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# codingcompetitions.withgoogle.com/kickstart/round/0000000000050ee1

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

${BINDIR}/candies: obj.d/candies.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

candies-test: ${BINDIR}/candies candies-tiny.in candies-tiny.out \
		candies-tiny2.in candies-tiny2.out
	timeout 2 ${BINDIR}/candies -naive candies-tiny.in candies-tiny.xnout
	diff candies-tiny.xnout candies-tiny.out
	timeout 2 ${BINDIR}/candies candies-tiny.in candies-tiny.xout
	diff candies-tiny.xout candies-tiny.out
	timeout 2 ${BINDIR}/candies -naive candies-tiny2.in candies-tiny2.xnout
	diff candies-tiny2.xnout candies-tiny2.out
	timeout 2 ${BINDIR}/candies candies-tiny2.in candies-tiny2.xout
	diff candies-tiny2.xout candies-tiny2.out

candies-internal-test: ${BINDIR}/candies
	timeout 12 ${BINDIR}/candies test 42 1 20 0 6 -9 9 10 11 12 13 0x10 10
	timeout 12 ${BINDIR}/candies test 6 500000 500000 \
	    0 6 -9 9 10 11 12 13 0x10 10

${BINDIR}/paraglide: obj.d/paraglide.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

paraglide-test: ${BINDIR}/paraglide paraglide-tiny.in paraglide-tiny.out
	timeout 2 ${BINDIR}/paraglide -naive paraglide-tiny.in paraglide-tiny.xnout
	diff paraglide-tiny.xnout paraglide-tiny.out
	timeout 2 ${BINDIR}/paraglide paraglide-tiny.in paraglide-tiny.xout
	diff paraglide-tiny.xout paraglide-tiny.out

${BINDIR}/funniest: obj.d/funniest.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

funniest-test: funniest-test1 funniest-test2

funniest-test1: ${BINDIR}/funniest funniest-tiny.in funniest-tiny.out
	timeout 2 ${BINDIR}/funniest -naive funniest-tiny.in funniest-tiny.xnout
	diff funniest-tiny.xnout funniest-tiny.out
	timeout 2 ${BINDIR}/funniest funniest-tiny.in funniest-tiny.xout
	diff funniest-tiny.xout funniest-tiny.out

funniest-test2: ${BINDIR}/funniest funniest-tiny2.in funniest-tiny2.out
	timeout 2 ${BINDIR}/funniest -naive funniest-tiny2.in funniest-tiny2.xnout
	diff funniest-tiny2.xnout funniest-tiny2.out
	timeout 2 ${BINDIR}/funniest funniest-tiny2.in funniest-tiny2.xout
	diff funniest-tiny2.xout funniest-tiny2.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
