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

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	timeout 2 ${BINDIR}/Dxxx -naive Dxxx-tiny.in Dxxx-tiny.xnout
	diff Dxxx-tiny.xnout Dxxx-tiny.out
	timeout 2 ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

${BINDIR}/Exxx: obj.d/Exxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Exxx-test: ${BINDIR}/Exxx Exxx-tiny.in Exxx-tiny.out
	timeout 2 ${BINDIR}/Exxx -naive Exxx-tiny.in Exxx-tiny.xnout
	diff Exxx-tiny.xnout Exxx-tiny.out
	timeout 2 ${BINDIR}/Exxx Exxx-tiny.in Exxx-tiny.xout
	diff Exxx-tiny.xout Exxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
