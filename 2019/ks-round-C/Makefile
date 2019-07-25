#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/May/26
# codingcompetitions.withgoogle.com/kickstart/round/0000000000050ff2/

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

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

${BINDIR}/Bxxx: obj.d/Bxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Bxxx-test: ${BINDIR}/Bxxx Bxxx-tiny.in Bxxx-tiny.out
	timeout 8 ${BINDIR}/Bxxx -naive Bxxx-tiny.in Bxxx-tiny.xnout
	diff Bxxx-tiny.xnout Bxxx-tiny.out
	timeout 8 ${BINDIR}/Bxxx Bxxx-tiny.in Bxxx-tiny.xout
	diff Bxxx-tiny.xout Bxxx-tiny.out

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

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
