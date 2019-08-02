#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/May/26
# codingcompetitions.withgoogle.com/kickstart/round/0000000000050ff2/

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu

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

${BINDIR}/wiggle: obj.d/wiggle.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

wiggle-test: ${BINDIR}/wiggle wiggle-tiny.in wiggle-tiny.out
	timeout 8 ${BINDIR}/wiggle -naive wiggle-tiny.in wiggle-tiny.xnout
	diff wiggle-tiny.xnout wiggle-tiny.out
	timeout 8 ${BINDIR}/wiggle wiggle-tiny.in wiggle-tiny.xout
	diff wiggle-tiny.xout wiggle-tiny.out

${BINDIR}/cboard: obj.d/cboard.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

cboard-test: cboard-test1 cboard-test2

cboard-test1: ${BINDIR}/cboard cboard-tiny.in cboard-tiny.out
	timeout 8 ${BINDIR}/cboard -naive cboard-tiny.in cboard-tiny.xnout
	diff cboard-tiny.xnout cboard-tiny.out
	timeout 8 ${BINDIR}/cboard cboard-tiny.in cboard-tiny.xout
	diff cboard-tiny.xout cboard-tiny.out

cboard-test2: ${BINDIR}/cboard cboard-tiny2.out cboard-tiny2.out
	timeout 8 ${BINDIR}/cboard -naive cboard-tiny2.in cboard-tiny2.xnout
	diff cboard-tiny2.xnout cboard-tiny2.out
	timeout 8 ${BINDIR}/cboard cboard-tiny2.in cboard-tiny2.xout
	diff cboard-tiny2.xout cboard-tiny2.out

${BINDIR}/Cxxx: obj.d/Cxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Cxxx-test: ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.out
	timeout 8 ${BINDIR}/Cxxx -naive Cxxx-tiny.in Cxxx-tiny.xnout
	diff Cxxx-tiny.xnout Cxxx-tiny.out
	timeout 8 ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.xout
	diff Cxxx-tiny.xout Cxxx-tiny.out

${BINDIR}/intervals: obj.d/intervals.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

intervals-test: ${BINDIR}/intervals
	${BINDIR}/intervals explicit -74 -72 -74
	${BINDIR}/intervals explicit -70 -71 -77 -71
	${BINDIR}/intervals explicit +6 +6 -6 +5
	${BINDIR}/intervals explicit +73 -73 +76 +73
	${BINDIR}/intervals explicit +73 -73 +76 -72 +73
	${BINDIR}/intervals explicit -72 -70 +73
	${BINDIR}/intervals explicit +73 -73 -72 +73
	${BINDIR}/intervals explicit +73 -77 -73 +76
	${BINDIR}/intervals random 20 4 70 80
	${BINDIR}/intervals random 20 5 70 80
	${BINDIR}/intervals random 20 10 70 80

${BINDIR}/maxbndrect: obj.d/maxbndrect.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

maxbndrect-test: ${BINDIR}/maxbndrect
	$< explicit 2 3 7 8 7
	$< explicit 2 3 4 8 1
	$< explicit 3 2 5
	$< explicit 2 6 4
	$< explicit 1 4 2
	$< explicit 4 3
	$< explicit 0 4
	$< explicit 2 2
	$< explicit 3 1
	$< explicit 1 2
	$< explicit 0
	$< explicit 3
	$< rand 100 1 4
	$< rand 100 2 4
	$< rand 100 3 6
	$< rand 10 10 100

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
