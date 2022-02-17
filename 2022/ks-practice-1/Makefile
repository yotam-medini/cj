#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# https://codingcompetitions.withgoogle.com/kickstart/round/00000000008f4332

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

${BINDIR}/ruler: obj.d/ruler.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

ruler-test: ${BINDIR}/ruler ruler-tiny.in ruler-tiny.out
	timeout 2 ${BINDIR}/ruler < ruler-tiny.in > ruler-tiny.xnout
	diff ruler-tiny.xnout ruler-tiny.out

${BINDIR}/hindex: obj.d/hindex.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

hindex-test: ${BINDIR}/hindex hindex-tiny.in hindex-tiny.out
	timeout 2 ${BINDIR}/hindex -naive hindex-tiny.in hindex-tiny.xnout
	diff hindex-tiny.xnout hindex-tiny.out
	timeout 2 ${BINDIR}/hindex hindex-tiny.in hindex-tiny.xout
	diff hindex-tiny.xout hindex-tiny.out

${BINDIR}/hex: obj.d/hex.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

hex-test: ${BINDIR}/hex hex-tiny.in hex-tiny.out
	timeout 2 ${BINDIR}/hex -naive hex-tiny.in hex-tiny.xnout
	diff hex-tiny.xnout hex-tiny.out
	timeout 2 ${BINDIR}/hex hex-tiny.in hex-tiny.xout
	diff hex-tiny.xout hex-tiny.out

${BINDIR}/milk: obj.d/milk.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

milk-test: ${BINDIR}/milk milk-tiny.in milk-tiny.out
	timeout 2 ${BINDIR}/milk -naive milk-tiny.in milk-tiny.xnout
	diff milk-tiny.xnout milk-tiny.out
	timeout 2 ${BINDIR}/milk milk-tiny.in milk-tiny.xout
	diff milk-tiny.xout milk-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
