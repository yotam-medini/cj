#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2022/August/27
# codingcompetitions.withgoogle.com/kickstart/round/00000000008f49d7

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

${BINDIR}/sample: obj.d/sample.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

sample-test: ${BINDIR}/sample sample-tiny.in sample-tiny.out
	timeout 2 ${BINDIR}/sample -naive sample-tiny.in sample-tiny.xnout
	diff sample-tiny.xnout sample-tiny.out
	timeout 2 ${BINDIR}/sample sample-tiny.in sample-tiny.xout
	diff sample-tiny.xout sample-tiny.out

${BINDIR}/record: obj.d/record.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

record-test: ${BINDIR}/record record-tiny.in record-tiny.out
	timeout 2 ${BINDIR}/record -naive record-tiny.in record-tiny.xnout
	diff record-tiny.xnout record-tiny.out
	timeout 2 ${BINDIR}/record record-tiny.in record-tiny.xout
	diff record-tiny.xout record-tiny.out

${BINDIR}/wiggle: obj.d/wiggle.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

wiggle-test: ${BINDIR}/wiggle wiggle-tiny.in wiggle-tiny.out
	timeout 2 ${BINDIR}/wiggle -naive wiggle-tiny.in wiggle-tiny.xnout
	diff wiggle-tiny.xnout wiggle-tiny.out
	timeout 2 ${BINDIR}/wiggle wiggle-tiny.in wiggle-tiny.xout
	diff wiggle-tiny.xout wiggle-tiny.out

${BINDIR}/gbus: obj.d/gbus.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

gbus-test: ${BINDIR}/gbus gbus-tiny.in gbus-tiny.out
	timeout 2 ${BINDIR}/gbus -naive gbus-tiny.in gbus-tiny.xnout
	diff gbus-tiny.xnout gbus-tiny.out
	timeout 2 ${BINDIR}/gbus gbus-tiny.in gbus-tiny.xout
	diff gbus-tiny.xout gbus-tiny.out

${BINDIR}/sherlock: obj.d/sherlock.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

sherlock-test: ${BINDIR}/sherlock sherlock-tiny.in sherlock-tiny.out
	timeout 2 ${BINDIR}/sherlock -naive sherlock-tiny.in sherlock-tiny.xnout
	diff sherlock-tiny.xnout sherlock-tiny.out
	timeout 2 ${BINDIR}/sherlock sherlock-tiny.in sherlock-tiny.xout
	diff sherlock-tiny.xout sherlock-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
