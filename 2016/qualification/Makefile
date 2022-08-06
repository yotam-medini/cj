#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2016/March/20
# codingcompetitions.withgoogle.com/codejam/round/0000000000201bee

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

LIBDIR=/usr/lib/i386-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

DBGFLAGS = -g
CFLAGS = ${DBGFLAGS} -Wall -std=c++11 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/recycle: obj.d/recycle.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

recycle-test: ${BINDIR}/recycle recycle-tiny.in recycle-tiny.out
	${BINDIR}/recycle recycle-tiny.in recycle-tiny.xout
	diff recycle-tiny.xout recycle-tiny.out

${BINDIR}/counting: obj.d/counting.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

counting-test: ${BINDIR}/counting counting-tiny.in counting-tiny.out
	${BINDIR}/counting counting-tiny.in counting-tiny.xout
	diff counting-tiny.xout counting-tiny.out

${BINDIR}/pancakes: obj.d/pancakes.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

pancakes-test: ${BINDIR}/pancakes pancakes-tiny.in pancakes-tiny.out
	${BINDIR}/pancakes pancakes-tiny.in pancakes-tiny.xout
	diff pancakes-tiny.xout pancakes-tiny.out

${BINDIR}/coin: obj.d/coin.o obj.d/get_primes.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

coin-test: ${BINDIR}/coin coin-tiny.in coin-tiny.out
	${BINDIR}/coin coin-tiny.in coin-tiny.xout
	diff coin-tiny.xout coin-tiny.out

${BINDIR}/fractiles: obj.d/fractiles.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

fractiles-test: ${BINDIR}/fractiles fractiles-tiny.in fractiles-tiny.out
	${BINDIR}/fractiles fractiles-tiny.in fractiles-tiny.xout
	diff fractiles-tiny.xout fractiles-tiny.out

${BINDIR}/get_primes_test: get_primes.cc
	@mkdir -p $(@D)
	g++ ${CFLAGS} -DTEST=1 -o $@ $^

get-primes-test: ${BINDIR}/get_primes_test
	${BINDIR}/get_primes_test
	${BINDIR}/get_primes_test 100


clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}


