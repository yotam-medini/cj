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
CFLAGS_COMMON = -Wall -Wshadow -std=c++17 -MMD
CFLAGS = ${OPTFLAGS} ${CFLAGS_COMMON}
COPTFLAGS = -O3 ${CFLAGS_COMMON}

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

obj-opt.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${COPTFLAGS} -o $@ $<

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

longest-random-test: ${BINDIR}/longest
	${BINDIR}/longest test 10 4 4 5
	${BINDIR}/longest test 10 2 10 5

${BINDIR}/primes: obj.d/primes.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

${BINDIR}/primes-opt: obj-opt.d/primes.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

primes-test: ${BINDIR}/primes ${BINDIR}/primes-opt primes-tiny.in primes-tiny.out
	timeout 2 ${BINDIR}/primes -naive primes-tiny.in primes-tiny.xnout
	diff primes-tiny.xnout primes-tiny.out
	timeout 6 ${BINDIR}/primes primes-tiny.in primes-tiny.xout
	diff primes-tiny.xout primes-tiny.out

.PHONY: primes-test-internal
primes-test-internal: ${BINDIR}/primes
	timeout 6 ${BINDIR}/primes test $$(seq 6 100)
	timeout 6 ${BINDIR}/primes test 99990 100000
	timeout 6 ${BINDIR}/primes test  999999999999999900 1000000000000000000

${BINDIR}/truck: obj.d/truck.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

truck-test: ${BINDIR}/truck truck-tiny.in truck-tiny.out
	timeout 2 ${BINDIR}/truck -naive truck-tiny.in truck-tiny.xnout
	diff truck-tiny.xnout truck-tiny.out
	timeout 2 ${BINDIR}/truck truck-tiny.in truck-tiny.xout
	diff truck-tiny.xout truck-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
