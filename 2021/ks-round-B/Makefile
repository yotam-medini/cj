#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/0000000000435a5b

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

${BINDIR}/incsub: obj.d/incsub.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

incsub-test: ${BINDIR}/incsub incsub-tiny.in incsub-tiny.out
	timeout 2 ${BINDIR}/incsub -naive incsub-tiny.in incsub-tiny.xnout
	diff incsub-tiny.xnout incsub-tiny.out
	timeout 2 ${BINDIR}/incsub incsub-tiny.in incsub-tiny.xout
	diff incsub-tiny.xout incsub-tiny.out

${BINDIR}/longest: obj.d/longest.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

longest-test: ${BINDIR}/longest longest-tiny.in longest-tiny.out
	timeout 2 ${BINDIR}/longest -naive longest-tiny.in longest-tiny.xnout
	diff longest-tiny.xnout longest-tiny.out
	timeout 2 ${BINDIR}/longest longest-tiny.in longest-tiny.xout
	diff longest-tiny.xout longest-tiny.out

${BINDIR}/primes: obj.d/primes.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

primes-test: ${BINDIR}/primes primes-tiny.in primes-tiny.out
	timeout 2 ${BINDIR}/primes -naive primes-tiny.in primes-tiny.xnout
	diff primes-tiny.xnout primes-tiny.out
	timeout 2 ${BINDIR}/primes primes-tiny.in primes-tiny.xout
	diff primes-tiny.xout primes-tiny.out

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
